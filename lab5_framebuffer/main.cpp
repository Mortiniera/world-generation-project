// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"
#include "glm/gtc/matrix_transform.hpp"

#include "framebuffer.h"

#include "skybox/skybox.h"
#include "water/water.h"
#include "grid/grid.h"
#include "screenquad/screenquad.h"
#include "framebuffer.h"
#include "camera.h"

mat4 mirror_view_matrix;
mat4 model_matrix;
mat4 sky_matrix = IDENTITY_MATRIX;

vec3 mirror_cam_pos;
vec3 mirror_cam_look;
//Quad quad;
Camera camera;
Mode mode = normal;
float depth = 0.0f;
float bezier_time;
bool mirror = false;

int window_width = 1200;
int window_height = 720;


using namespace glm;

mat4 projection_matrix;
mat4 view_matrix;
    
//mat4 cube_model_matrix;

mat4 decalage = IDENTITY_MATRIX;

vec3 cam_pos(2.0f, 0.5f, -2.0f);
vec3 cam_look(0.0f, 0.0f, 0.0f);
vec3 cam_up(0.0f, 1.0f, 0.0f);
vec3 cam_right;
float t_time = glfwGetTime();
int day_time = 0; //afternoon


bool polygon = false;

int terrain_param = 3;
int tex_width = 1000;

const int terrain_grid_number = 9; //2*pow(i, 2) + (2*i-3)

Grid terrain[terrain_grid_number];
Skybox sky;
FrameBuffer frame_array[terrain_grid_number];
FrameBuffer framewater_array[terrain_grid_number];
ScreenQuad perlin_array[terrain_grid_number];
Water water[terrain_grid_number];
GLuint texture_id_array[terrain_grid_number];
GLuint water_id_array[terrain_grid_number];

void Init(GLFWwindow* window) {
    glClearColor(1.0, 1.0, 1.0 /*white*/, 1.0 /*solid*/);
    glEnable(GL_DEPTH_TEST);
    sky.Init();

    sky_matrix = scale(sky_matrix,vec3(6.0f));
    model_matrix = IDENTITY_MATRIX;

    cam_look = normalize(cam_pos-cam_look);
    cam_right = normalize(cross(cam_up, cam_look));

    camera.Init(cam_up, cam_look, cam_pos, cam_right, mode);
    
    float ratio = window_width / (float) window_height;
    projection_matrix = perspective(45.0f, ratio, 0.1f, 20.0f);

    
    //boucle pour initialiser les 9 grid et les 9 "water"
    for (int i = 0; i<terrain_grid_number; ++i){
        texture_id_array[i] = frame_array[i].Init(tex_width, tex_width, true);
        perlin_array[i].Init(tex_width, tex_width, texture_id_array[i], int(i/terrain_param), int(i%terrain_param));

        frame_array[i].Bind();
            glViewport(0, 0,tex_width, tex_width);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            perlin_array[i].Draw();
        frame_array[i].Unbind();


        terrain[i].Init(texture_id_array[i]);

    }

    water_id_array[0] = framewater_array[0].Init(window_width, window_height, true);
    water[0].Init(water_id_array[0], window_width, window_height);

}


void Display() { //water[i].Init(water_id_array[i], window_width, window_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //j'ai ajoute cela
    glViewport(0, 0, window_width, window_height);

    if(mode == first_person){
        vec3 data = camera.frame_index_and_off();
        int index = data.x;    //data.x contains index grid

        frame_array[index].Bind();
            glViewport(0, 0,tex_width, tex_width);
            glReadPixels((data.y+1)*0.5*tex_width, (data.z+1)*0.5*tex_width, 1, 1, GL_RED, GL_FLOAT, &depth); //data.y contains x_off, data.z contains z_off
            //printf("%s %f %s %f\n","x_off", data.y,"z_off", data.z);
            //printf("%f\n", depth);
        frame_array[index].Unbind();
        glViewport(0, 0,window_width, window_height);

        camera.set_first_person_depth(depth+0.2);
    }
    if(mode == bezier){
        float diff = glfwGetTime()-bezier_time;
       //printf("%s %f\n", "diff", diff);
       if(diff < 30){
           //printf("%s %f\n", "t : " , diff/10);
           camera.generic_bezier(diff/30.0, 3);
       }
       else if(diff >= 30){
           mode = normal;
           //printf("%s\n", "mode normal");
       }
    }


    view_matrix = camera.get_view_matrix(mode);

    mat4 t = IDENTITY_MATRIX;
    cam_pos = camera.get_cam_pos();
    t[3] = vec4(-0.5+cam_pos.x, cam_pos.y, 0.5+cam_pos.z, 1.0); //translation matrix
    sky.Draw(sky_matrix,view_matrix*t,projection_matrix, day_time);

    for (int i = 0; i<terrain_grid_number; ++i){
        decalage[3] = vec4((2)*int(i/terrain_param), 0.0f, -(2)*int(i%terrain_param), 1.0f);
        terrain[i].Draw(IDENTITY_MATRIX, view_matrix*decalage , projection_matrix, polygon, mirror);
    }

    //mirroring
    vec3 new_cam_pos = camera.get_cam_pos();
    new_cam_pos.y *= -1;
    vec3 new_cam_look = camera.get_cam_look();
    new_cam_look.y *= -1;
    vec3 new_cam_up = camera.get_cam_up();
    mat4 new_view = lookAt(new_cam_pos, new_cam_pos -  new_cam_look, new_cam_up);

    mirror = true;
    framewater_array[0].Bind();
        glViewport(0, 0,window_width, window_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        sky.Draw(sky_matrix,view_matrix*t,projection_matrix, day_time);
        for (int i = 0; i<terrain_grid_number; ++i){
            decalage[3] = vec4((2)*int(i/terrain_param), 0.0f, -(2)*int(i%terrain_param), 1.0f);
            terrain[i].Draw(IDENTITY_MATRIX, new_view * decalage, projection_matrix, mirror);
        }
    framewater_array[0].Unbind();
    mirror = false;
    glViewport(0, 0, window_width, window_height);
    mat4 trans = IDENTITY_MATRIX;
    trans[3] = vec4(-2/float(terrain_param), 0.0, 2/float(terrain_param), 1.0);
    water[0].Draw(scale(IDENTITY_MATRIX, vec3(terrain_param))*trans, view_matrix*decalage , projection_matrix, polygon);
}

// gets called when the windows/framebuffer is resized.
void ResizeCallback(GLFWwindow* window, int width, int height) {
    window_width = width;
    window_height = height;
    glfwGetFramebufferSize(window, &window_width, &window_height);

    float ratio = window_width / (float) window_height;
    projection_matrix = perspective(45.0f, ratio, 0.1f, 10.0f);

    glViewport(0, 0, window_width, window_height);
    framewater_array[0].Cleanup();
    water_id_array[0] = framewater_array[0].Init(window_width, window_height);
    water[0].Init(water_id_array[0], window_width, window_height);
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
    }
    else if (key == GLFW_KEY_N & action == GLFW_PRESS){
        mode = normal;
    }
    else if (key == GLFW_KEY_M & action == GLFW_PRESS){
        mode = first_person;
    }
    else if (key == GLFW_KEY_B & action == GLFW_PRESS){
        bezier_time = glfwGetTime();
        mode = bezier;
    }
    else if (key == GLFW_KEY_0 & action == GLFW_PRESS) { //afternoon
            day_time = 0;
    }
    else if (key == GLFW_KEY_1 & action == GLFW_PRESS) { //sunset
            day_time = 1;
    }
    else if (key == GLFW_KEY_2 & action == GLFW_PRESS) { //night
            day_time = 2;
    }

    camera.update_by_key(key, action);

}

int main(int argc, char *argv[]) {
    // GLFW Initialization
    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return EXIT_FAILURE;
    }

    glfwSetErrorCallback(ErrorCallback);

    // hint GLFW that we would like an OpenGL 3 context (at least)
    // http://www.glfw.org/faq.html#how-do-i-create-an-opengl-30-context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // attempt to open the window: fails if required version unavailable
    // note some Intel GPUs do not support OpenGL 3.2
    // note update the driver of your graphic card
    GLFWwindow* window = glfwCreateWindow(window_width, window_height,
                                          "framebuffer", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // makes the OpenGL context of window current on the calling thread
    glfwMakeContextCurrent(window);

    // set the callback for escape key
    glfwSetKeyCallback(window, KeyCallback);
    //glfwSetKeyCallback(window, camera.key);

    // set the framebuffer resize callback
    glfwSetFramebufferSizeCallback(window, ResizeCallback);

    // GLEW Initialization (must have a context)
    // https://www.opengl.org/wiki/OpenGL_Loading_Library
    glewExperimental = GL_TRUE; // fixes glew error (see above link)
    if(glewInit() != GLEW_NO_ERROR) {
        fprintf( stderr, "Failed to initialize GLEW\n");
        return EXIT_FAILURE;
    }

    cout << "OpenGL" << glGetString(GL_VERSION) << endl;

    // initialize our OpenGL program
    Init(window);

    // render loop
    while(!glfwWindowShouldClose(window)){
        Display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // cleanup
    sky.Cleanup();
    
    for (int i = 0; i<terrain_grid_number; ++i){
       terrain[i].Cleanup();
       frame_array[i].Cleanup();
       perlin_array[i].Cleanup();
    }

    framewater_array[0].Cleanup();
    water[0].Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
