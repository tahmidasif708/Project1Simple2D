/**
* Author: Tahmid Asif
* Assignment: Simple 2D Scene
* Date due: 2023-09-30, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"

using namespace std;

//const int WINDOW_WIDTH = 640,
          //WINDOW_HEIGHT = 480;

/*
const float BG_RED = 0.1922f,
            BG_BLUE = 0.549f,
            BG_GREEN = 0.9059f,
            BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;
*/

const char PLAYER_SPRITE1_FILEPATH[] = "/Users/tahmidasif/Desktop/2Dproj/2Dproj/lioness.png";
const char PLAYER_SPRITE2_FILEPATH[] = "/Users/tahmidasif/Desktop/2Dproj/2Dproj/cub.png";
const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_shader_program;
//Lioness
glm::mat4 view_matrix, m_model_matrix, m_projection_matrix;

//Cubs
glm::mat4 m_model_matrix1, m_model_matrix2, m_model_matrix3;

float g_player_x = 0;
float g_player_y = 0;
float g_player_z = 0;
float g_player_rotate = 0;

GLuint lioness_textureID;
GLuint cub_textureID;

/*
const int TRIANGLE_RED     = 1.0,
          TRIANGLE_BLUE    = 0.4,
          TRIANGLE_GREEN   = 0.4,
          TRIANGLE_OPACITY = 1.0;

const float GROWTH_FACTOR = 1.01f;
const float SHRINK_FACTOR = 0.99f;
const int MAX_FRAME       = 40;

const float ROT_ANGLE  = glm::radians(1.5f);
const float TRAN_VALUE = 0.025f;
*/

bool g_is_growing = true;
int  g_frame_counter = 0;

glm::mat4 g_view_matrix,
          g_model_matrix,
          g_projection_matrix,
          g_tran_matrix;

GLuint load_texture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL)
        {
            std::cout << "Unable to load image. Make sure the path is correct.\n";
            std::cout << filePath << "\n";
            assert(false);
        }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    
    return textureID;
}

void initialise(){
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("A Lioness And Her Playful Cubs!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL); // 640, 480 ?*
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480); // 640, 480 ?*
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    view_matrix       = glm::mat4(1.0f);
    m_model_matrix      = glm::mat4(1.0f);
    m_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_shader_program.set_view_matrix(view_matrix);
    g_shader_program.set_projection_matrix(m_projection_matrix);
    
    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(0.133f, 0.545f, 0.133f, 1.0f);
    
    //TextureID
    lioness_textureID = load_texture(PLAYER_SPRITE1_FILEPATH);
    cub_textureID = load_texture(PLAYER_SPRITE2_FILEPATH);
    
    //Blend
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
}

void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
        {
            g_game_is_running = false;
        }
    }
}

float m_previous_ticks = 0.0f;

void update(){
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - m_previous_ticks;
    m_previous_ticks = ticks;
    
    g_player_x += 0.63f * deltaTime;
    g_player_y += 0.63f * deltaTime;
    g_player_rotate += 90.0f * deltaTime; //90 degrees per sec
    g_player_z += 1.01 * deltaTime;
    
    //lioness
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, glm::vec3(0.0f, -1.0f, 0.0f));
    m_model_matrix = glm::scale(m_model_matrix, glm::vec3(4, 4, 1.0f));
    m_model_matrix = glm::translate(m_model_matrix, glm::vec3(sin(1.4*g_player_x), 0.0f, 0.0f));
    
    //cub 1, cub 2, cub 3
    m_model_matrix1 = glm::mat4(1.0f);
    m_model_matrix1 = glm::translate(m_model_matrix1, glm::vec3(1.85f, -1.25f, 0.0f));
    m_model_matrix1 = glm::translate(m_model_matrix1, glm::vec3(1.85f, sin(20 * g_player_x), 0.0f));
    
    // Calculate relative position of Cub 2
    glm::vec3 cub2RelativePosition = glm::vec3(sin(1.4 * g_player_x), 0.0f, 0.0f); // Adjust as needed

    // Update Cub 2's position based on Lioness
    m_model_matrix2 = glm::mat4(1.0f);
    m_model_matrix2 = glm::translate(m_model_matrix2, glm::vec3(0.85f, 0.2f, 0.0f)); // Initial position
    m_model_matrix2 = glm::translate(m_model_matrix2, cub2RelativePosition); // Apply relative position
        
    m_model_matrix3 = glm::mat4(1.0f);
    m_model_matrix3 = glm::translate(m_model_matrix3, glm::vec3(1.35f, -1.25f, 0.0f));
    m_model_matrix3 = glm::translate(m_model_matrix3, glm::vec3(1.35f, sin(10 * g_player_x), 0.0f));
    m_model_matrix3 = glm::rotate(m_model_matrix3, glm::radians(g_player_rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    
}

void render(){
    glClear(GL_COLOR_BUFFER_BIT);
    
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texture_coordinates[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    //lioness
    g_shader_program.set_model_matrix(m_model_matrix);
    
    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());
    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    glBindTexture(GL_TEXTURE_2D, lioness_textureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    //cub1
    g_shader_program.set_model_matrix(m_model_matrix1);
    
    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());
    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    glBindTexture(GL_TEXTURE_2D, cub_textureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    //cub2
    g_shader_program.set_model_matrix(m_model_matrix2);
    
    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());
    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    glBindTexture(GL_TEXTURE_2D, cub_textureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    //cub3
    g_shader_program.set_model_matrix(m_model_matrix3);
    
    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());
    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    glBindTexture(GL_TEXTURE_2D, cub_textureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    initialise();
    
    while (g_game_is_running)
    {
        process_input();
        update();
        render();
    }
    
    shutdown();
    return 0;
}

