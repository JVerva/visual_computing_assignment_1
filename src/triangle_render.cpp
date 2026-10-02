/* ------------------------------------------------------------------------- */
/* ---- INCLUDES ----------------------------------------------------------- */
/*
 * Include standard headers
 */

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

/* 
 * Include Glad
 */
 
#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
/*
 * Include GLFW 
 * Multi-platform library for creating windows, contexts and surfaces, receiving input and events.
 */
#include <GLFW/glfw3.h>
GLFWwindow* window;

/* 
 *Include GLM - OpenGL Maths
 */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

//----------------------------------------------------------------------------------------

enum FILTER{
    NONE,
    GRAYSCALE,
    GAUSSIAN_BLUR,
    SOBEL,
    SHARPEN
};

const cv::Mat GAUSSIAN_BLUR_KERNAL = (cv::Mat_<float>(3,3) << 1.0f/16, 2.0f/16, 1.0f/16, 2.0f/16, 4.0f/16, 2.0f/16, 1.0f/16, 2.0f/16, 1.0f/16);
const cv::Mat SOBEL_X_KERNAL = (cv::Mat_<float>(3,3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
const cv::Mat SOBEL_Y_KERNAL = (cv::Mat_<float>(3,3) << -1, -2, -1, 0, 0, 0, -1, -2, -1);
const cv::Mat SHARPEN_KERNAL = (cv::Mat_<float>(3,3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);

// function declaration from the opencv proj

void choose_filter(GLFWwindow* window, FILTER& filter);
void transform_frame(cv::Mat in_frame, cv::Mat& out_frame, FILTER filter);
void processInput();
void choose_filter(FILTER& filter);
std::string loadFile(const std::string& path);

//----------------------------------------------------------------------------------------

//open gl declarations
class Camera;
GLuint create_shader_program(std::string vert_path, std::string frag_path);
bool initWindow(std::string windowName);
void moveCamera(Camera& camera, float speed, float delta);

class Camera{
    private:
        // view matrix vars
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        // projection matrix vars
        glm::float64 aspectRatio = 16.0f/9.0f;
        glm::float64 closePlaneDistance = 0.1f;
        glm::float64 farPlaneDistance = 100.0f;

    public:
        glm::vec3 cameraPos = glm::vec3(0.0f, 3.0f, 3.0f);
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::float64 fov = 45.0f;

        Camera(){
        }
        Camera(glm::vec3 cameraPos_, glm::vec3 cameraTarget_, glm::vec3 upDirection_, glm::float64 fov_, glm::float64 aspectRatio_, glm::float64 closePlaneDistance_, glm::float64 farPlaneDistance_){
            cameraPos = cameraPos_;
            cameraTarget = cameraTarget_;
            up = upDirection_;
            fov = fov_;
            closePlaneDistance = closePlaneDistance_;
            farPlaneDistance = farPlaneDistance_;
        }


        void setFOV(glm::float64 fov_){
            fov = fov_;
        }

        glm::vec3 getCameraDirection(){
            return glm::normalize(cameraPos - cameraTarget);
        }

        glm::vec3 getCameraRight(){
            return glm::normalize(glm::cross(up, getCameraDirection()));
        }

        glm::vec3 getCameraUp(){
            return glm::normalize(glm::cross(getCameraDirection(), getCameraRight()));
        }

        glm::vec3 getCameraFront(){
            return -getCameraDirection();
        }
        
        glm::mat4 getViewMatrix(){
            return glm::lookAt(cameraPos, cameraPos + getCameraFront(), getCameraUp());
        }

        glm::mat4 getProjectionMartix(){
            return glm::perspective(fov, aspectRatio, closePlaneDistance, farPlaneDistance);
        }
};

/* ---- Helper Functions  ------------------------------------------------------- */

/*
 *  initWindow
 *
 *  This is used to set up a simple window using GLFW.
 *  Returns true if sucessful otherwise false.
 */
bool initWindow(std::string windowName){
    
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return false;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, windowName.c_str(), NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. .\n" );
        getchar();
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    
    return true;
    
}

/*
 *  main
 *
 *  This is the main function that does all the work.
 *  Creates the window and than calls renderLoop.
 */
int main( void )
{
	//Init a basic window
    bool windowInitSucess = initWindow("Triangle Render");
    if(!windowInitSucess){
        return -1;
    }

   if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return -1;
    }

    //open the default webcam (0)
    cv::VideoCapture cap(0);

    // Check if the video was opened successfully
    if (!cap.isOpened()) {
        fprintf(stderr, "Failed to open video capture\n");
        return -1;
    }

    Camera cam = Camera();
    //Representation of the 3 vertices of our triangle
    //An array of 3 vectors each consistin of x,y,z
    std::vector<glm::vec3> m_vertices = {
        {-0.5f, -0.5f, 0.0f},
        {0.5f, -0.5f, 0.0f},
        {0.0f, 0.5f, 0.0f},
    };
    //model matrix for triangle, to apply tranformations from local space to global space
    glm::mat4 model = glm::mat4(1.0f);

    //background texture coordinates
    std::vector<glm::vec2> texCoords = {
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f},
    };
    GLuint back_texture;
    glGenTextures(1, &back_texture);
    glBindTexture(GL_TEXTURE_2D, back_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    //create vaos and vbos
    GLuint VAOS[2];
    glGenVertexArrays(2, VAOS);
    //create vbos
    GLuint VBOS[2];
    glGenBuffers(2, VBOS);

    // background VAO/VBO setup
    GLuint back_VBO = VBOS[1];
    GLuint back_VAO = VAOS[1];
    glBindVertexArray(back_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, back_VBO);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // triangle VAO/VBO setup
    GLuint triangle_VAO = VAOS[0];
    //load vertice data into a VBO
    GLuint VBO = VBOS[0];
    glBindVertexArray(triangle_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), m_vertices.data(), GL_STATIC_DRAW);
    //specify vertex attribute pointer (how to read the vertex buffer, location, size, types, stride length, ...)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    GLuint triangle_program = create_shader_program("src/vert/triangle.vert", "src/frag/triangle.frag");
    GLuint back_program = create_shader_program("src/vert/back.vert", "src/frag/back.frag");

	// Ensure we can capture the escape and camera movement keys being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// green background
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    
    //background frame vars
    cv::Mat og_frame;
    cv::Mat trans_frame;
    FILTER filter = FILTER::NONE;

    //render loop
    // Check if the ESC key was pressed or the window was closed
    double deltaTime;
    double lastFrame = glfwGetTime();
    while(!glfwWindowShouldClose(window)){
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput();
        moveCamera(cam, 100, deltaTime);
        choose_filter(filter);

        bool ret = cap.read(og_frame);
        if (ret && !og_frame.empty()) {
            transform_frame(og_frame, trans_frame, filter);
            cv::flip(trans_frame, trans_frame, 0);

            cv::Mat rgb_frame;
            cv::cvtColor(trans_frame, rgb_frame, cv::COLOR_BGR2RGB);
            glBindTexture(GL_TEXTURE_2D, back_texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, rgb_frame.cols, rgb_frame.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb_frame.data);
        }

        // Clear the screen.
        glClear(GL_COLOR_BUFFER_BIT);
        //draw background
        //set shader program to use and bind VAO
        glUseProgram(back_program);
        glBindTexture(GL_TEXTURE_2D, back_texture);
        glBindVertexArray(back_VAO);
        glUniform1i(glGetUniformLocation(back_program, "ourTexture"), 0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        
        //draw triangle
        //set shader program to use and bind VAO
        glUseProgram(triangle_program);
        glBindVertexArray(triangle_VAO);     
        // pass in matrixes (model, view, projection) to the vertex shader
        int modelLoc = glGetUniformLocation(triangle_program, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        int viewLoc = glGetUniformLocation(triangle_program, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(cam.getViewMatrix()));
        int projectionLoc = glGetUniformLocation(triangle_program, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(cam.getProjectionMartix()));
        
        // Draw Triangle
        glDrawArrays(GL_TRIANGLES, 0, 3);
            
        // Swap buffers
        glfwSwapBuffers(window);
        //as for events
        glfwPollEvents();
        
    }

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

void transform_frame(cv::Mat in_frame, cv::Mat& out_frame, FILTER filter){
    //transfrom og frame
    cv::Mat s_x;
    cv::Mat s_y;
    switch(filter){
        case FILTER::NONE:
            out_frame = in_frame;
            break;
        case FILTER::GRAYSCALE:
            //convert to grayscale
            cv::cvtColor(in_frame, out_frame, cv::COLOR_BGR2GRAY);
            break;
        case FILTER::GAUSSIAN_BLUR:
            // convolute image with gaussian blur kernal
            cv::blur(in_frame, out_frame, cv::Size(5,5));
            break;
        case FILTER::SOBEL:
            // apply sobel x
            cv::Sobel(in_frame, out_frame, in_frame.depth(), 1, 1, 3, 3);
            break;
        case FILTER::SHARPEN:
            cv::filter2D(in_frame, out_frame, in_frame.depth(), SHARPEN_KERNAL);
            break;
            
        default:
            out_frame = in_frame;
            break;
    }
}

// open gl

GLuint create_shader_program(std::string vert_path, std::string frag_path){
    //load and compile vertex shader
    std::string vertSrc = loadFile(vert_path);
    const char* vertexShaderSource = vertSrc.c_str();
    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    //print shader compilation errors
    int success;
    char InfoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertexShader, sizeof(InfoLog), NULL, InfoLog);
        fprintf(stderr, "ERROR::SHADER::VERTEX::COMPILATION_FAILED:\n %s", InfoLog);
    }

    //load and compile fragment shader
    std::string fragSrc = loadFile(frag_path);
    const char* fragShaderSource = fragSrc.c_str();
    GLuint fragShader;
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragShaderSource, NULL);
    glCompileShader(fragShader);
    //print shader compilation errors
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragShader, sizeof(InfoLog), NULL, InfoLog);
        fprintf(stderr, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED:\n %s", InfoLog);
    }

    //create shader program with vertex and fragment shader
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);
    //print program linker errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(shaderProgram, sizeof(InfoLog), NULL, InfoLog);
        fprintf(stderr, "ERROR::SHADER::PROGRAM::LINKING_FAILED:\n %s", InfoLog);
    }
    //delete shader objects
    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);

    return shaderProgram;
}

//returns file content as a string
std::string loadFile(const std::string& path) {
    std::ifstream file(path);
    std::stringstream buf;
    buf << file.rdbuf();
    return buf.str();
}

void processInput()
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void moveCamera(Camera& camera, float speed, float delta){
    glm::vec3 moveDir = glm::vec3(0);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        moveDir += camera.getCameraFront();
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        moveDir -= camera.getCameraFront();
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        moveDir -= camera.getCameraRight();
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        moveDir += camera.getCameraRight();
    if (moveDir != glm::vec3(0))
        moveDir = glm::normalize(moveDir);
    camera.cameraPos += moveDir * delta * speed;
}

void choose_filter(FILTER& filter){
    if(glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
        filter = FILTER::NONE;
    if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        filter = FILTER::GRAYSCALE;
    if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        filter = FILTER::GAUSSIAN_BLUR;
    if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        filter = FILTER::SOBEL;
    if(glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        filter = FILTER::SHARPEN;
}
