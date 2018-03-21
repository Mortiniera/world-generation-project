#pragma once
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>

class Water {

    private:
        GLuint vertex_array_id_;                // vertex array object
        GLuint vertex_buffer_object_position_;  // memory buffer for positions
        GLuint vertex_buffer_object_index_;     // memory buffer for indices
        GLuint program_id_;                     // GLSL shader program ID
        GLuint texture_id_;                     // texture ID

        GLuint water_dim_id_;
        GLuint water_texture_id_;

        GLuint num_indices_;                    // number of vertices to render
        GLuint MVP_id_;                         // model, view, proj matrix ID
        GLuint MV_id_;                         // model, view matrix ID

        GLuint vertex_normal_buffer_object_;

        float water_width_;
        float water_height_;

        float water_dim = 400;



    public:
        void Init(GLuint tex_mirror, int water_width, int water_height) {

           this->water_width_ = water_width;
            this->water_height_ = water_height;

            // compile the shaders.
            program_id_ = icg_helper::LoadShaders("water_vshader.glsl",
                                                  "water_fshader.glsl");
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);
            /*light.Setup(program_id_);
            material.Setup(program_id_);*/

            // vertex one vertex array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // vertex coordinates and indices
            {
                std::vector<GLfloat> vertices;
                std::vector<GLuint> indices;

                //vertex position of the small triangles
                for (int i = 0; i <= water_dim; i++) {
                    float x = float(i) / (water_dim/2.0);
                    for(int j = 0; j <= water_dim; j++) {
                        float y = float(j) / (water_dim/2.0);
                        vertices.push_back(-1.0f + x);
                        vertices.push_back(-1.0f + y);
                    }
                }
                int primitive_start_index = 0xffffffff;
                glPrimitiveRestartIndex(primitive_start_index);
                glEnable(GL_PRIMITIVE_RESTART);

                for(int i = 0; i<= water_dim-1;i++){
                    for(int j = 0; j<=water_dim; j++){
                        indices.push_back(i*(water_dim+1) +j);
                        indices.push_back((i+1)*(water_dim+1) +j);
                    }
                    indices.push_back(primitive_start_index);
                }

                num_indices_ = indices.size();

                // position buffer
                glGenBuffers(1, &vertex_buffer_object_position_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_position_);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
                             &vertices[0], GL_STATIC_DRAW);

                // vertex indices
                glGenBuffers(1, &vertex_buffer_object_index_);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_index_);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
                             &indices[0], GL_STATIC_DRAW);

                // position shader attribute
                GLuint loc_position = glGetAttribLocation(program_id_, "vpoint");
                glEnableVertexAttribArray(loc_position);
                glVertexAttribPointer(loc_position, 2, GL_FLOAT, DONT_NORMALIZE,
                                      ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }

            // load texture

            this->texture_id_ = tex_mirror;
            glBindTexture(GL_TEXTURE_2D, texture_id_);
            GLuint tex_id = glGetUniformLocation(program_id_, "tex_mirror");
            glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);
            glBindTexture(GL_TEXTURE_2D, 0);

            {
                int width;
                int height;
                int nb_component;
                string filename = "water.jpg";
                // set stb_image to have the same coordinates as OpenGL
                stbi_set_flip_vertically_on_load(1);
                unsigned char* image = stbi_load(filename.c_str(), &width,
                                                 &height, &nb_component, 0);

                if(image == nullptr) {
                    throw(string("Failed to load texture"));
                }

                glGenTextures(1, &water_texture_id_);
                glBindTexture(GL_TEXTURE_2D, water_texture_id_);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

                if(nb_component == 3) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                                 GL_RGB, GL_UNSIGNED_BYTE, image);
                } else if(nb_component == 4) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                                 GL_RGBA, GL_UNSIGNED_BYTE, image);
                }

                GLuint water_tex_id = glGetUniformLocation(program_id_, "water_tex");
                glUniform1i(water_tex_id, 5 /*GL_TEXTURE0*/);

                // cleanup
                glBindTexture(GL_TEXTURE_2D, 5);
                stbi_image_free(image);
            }

            // to avoid the current object being polluted
            glBindVertexArray(0);
            glUseProgram(0);

            // other uniforms
            MV_id_ = glGetUniformLocation(program_id_, "MV");
            MVP_id_ = glGetUniformLocation(program_id_, "MVP");

            // to avoid the current object being polluted
            glBindVertexArray(0);
            glUseProgram(0);
        }

        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &vertex_buffer_object_position_);
            glDeleteBuffers(1, &vertex_buffer_object_index_);
            glDeleteVertexArrays(1, &vertex_array_id_);
            glDeleteProgram(program_id_);
            glDeleteTextures(1, &texture_id_);
        }


        void UpdateSize(int water_width, int water_height) {
                    this->water_width_ = water_width;
                    this->water_height_ = water_height;
        }

        void Draw(const glm::mat4 &model = IDENTITY_MATRIX, const glm::mat4 &view = IDENTITY_MATRIX, const glm::mat4 &projection = IDENTITY_MATRIX, bool pol=false) {
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // window size uniforms
            glUniform1f(glGetUniformLocation(program_id_, "tex_width"),this->water_width_);
            glUniform1f(glGetUniformLocation(program_id_, "tex_height"),this->water_height_);

            // bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id_);


            glm::vec3 light_pos = glm::vec3(0.0f, 2.0f, 2.0f);
            glm::vec3 La = glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3 Ld = glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3 Ls = glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3 ka = glm::vec3(0.18f/*8*/, 0.1f, 0.1f);
            glm::vec3 kd = glm::vec3(0.9f/*9*/, 0.5f, 0.5f);
            glm::vec3 ks = glm::vec3(0.8f, 0.8f, 0.8f);
            float alpha = 60.0f;
            // get ka, la... values
            float time = glfwGetTime();
            glUniform1f(glGetUniformLocation(program_id_, "time"), time);
            GLuint light_pos_id = glGetUniformLocation(program_id_, "light_pos");
            GLuint La_id = glGetUniformLocation(program_id_, "La");
            GLuint Ld_id = glGetUniformLocation(program_id_, "Ld");
            GLuint Ls_id = glGetUniformLocation(program_id_, "Ls");
            glUniform3fv(light_pos_id, ONE, glm::value_ptr(light_pos));
            glUniform3fv(La_id, ONE, glm::value_ptr(La));
            glUniform3fv(Ld_id, ONE, glm::value_ptr(Ld));
            glUniform3fv(Ls_id, ONE, glm::value_ptr(Ls));

            GLuint ka_id = glGetUniformLocation(program_id_, "ka");
            GLuint kd_id = glGetUniformLocation(program_id_, "kd");
            GLuint ks_id = glGetUniformLocation(program_id_, "ks");
            GLuint alpha_id = glGetUniformLocation(program_id_, "alpha");
            glUniform3fv(ka_id, ONE, glm::value_ptr(ka));
            glUniform3fv(kd_id, ONE, glm::value_ptr(kd));
            glUniform3fv(ks_id, ONE, glm::value_ptr(ks));
            glUniform1f(alpha_id, alpha);

            // setup MV
            glm::mat4 MV = view*model;
            glUniformMatrix4fv(MV_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MV));

            // setup MVP
            glm::mat4 MVP = projection*view*model;
            glUniformMatrix4fv(MVP_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MVP));

            if(pol==true){
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }


            glDrawElements(GL_TRIANGLE_STRIP, num_indices_, GL_UNSIGNED_INT, 0);

            glBindVertexArray(0);
            glUseProgram(0);
        }
};
