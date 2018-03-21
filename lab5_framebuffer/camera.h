#pragma once
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>
#include "math.h"

using namespace glm;

const float speed_ref = 10.0f;
float speed = 0.0f;
bool begin_acc = false;
bool begin_dec = false;
bool forward_dir = false;
bool backward_dir = false;
bool movement = false;
float rotate_factor = 0.0f;
float factor = 0.02f;
float movement_factor = 0.01f;
float dec_movement_factor = 0.01f;
bool keys[1024]; //utile pour bouger camera dans plusieurs sens en meme temps

vec3 rotation_axis = vec3(0.0f, 0.0f, 0.0f);

int current_grid;
float depth_first_person;

enum Mode { normal, first_person, bezier};
Mode this_mode;

int bin_coeff_2[] = {1, 2, 1};
int bin_coeff_3[] = {1,3,3,1};
int bin_coeff_4[] = {1,4,6,4,1};
int bin_coeff_5[] = {1,5,10,10,5,1};

int bin_coeff[] = {1,2,1,1,3,3,1,1,4,6,4,1,1,5,10,10,5,1,1,6,15,20,15,6,1};
int offset_bin_coeff[] = {0, 0, 0, 3, 7, 11, 16}; //bezier til order 5

vec3 bezier_control[] = {vec3(4.0, 1.0, -0.5), vec3(0.0, 0.5, -2.0), vec3(4.0, 0.5, -4.0),
        vec3(3.0, 1.0, -3.0), vec3(2.0, 0.5, -2.0), vec3(1.0, 0.3, -3.5), vec3(3.0, 0.5, -4.0)};

class Camera {
	private:
		vec3 cam_up;
		vec3 cam_look;
		vec3 cam_pos;
        vec3  cam_right;

		mat4 view_matrix;

	public:

		void Init(vec3 u, vec3 l, vec3 p, vec3 r, Mode mode){
			cam_up = u;
			cam_look = l;
			cam_pos = p;
			cam_right = r;
			view_matrix = lookAt(cam_pos, cam_pos-cam_look, cam_up);
			this_mode = mode;

			//printf("%f %f %f %f\n", bezier_control_4[0].z,bezier_control_4[1].z,bezier_control_4[2].z,bezier_control_4[3].z);
		}

		mat4 get_view_matrix(Mode mode){

			this_mode = mode;

		    if(begin_acc){
		        //printf("%s\n", "begin acc");
		        movement = true;
		        if(forward_dir){
		            if(speed < speed_ref){
                        //printf("%s %f\n", "speed < speed_ref", speed);
		                speed += dec_movement_factor;
		            }
		            else if(speed >= speed_ref){
                        //printf("%s %f\n", "speed >= speed_ref", speed);
		                speed = speed_ref;
		                begin_acc =  false;
		            }
		        }
		        else if(backward_dir){
		            if(speed > -speed_ref){
                        //printf("%s %f\n", "speed > -speed_ref", speed);
		                speed -= dec_movement_factor;
		            }
		            else if(speed <= -speed_ref){
                        //printf("%s %f\n", "speed <= -speed_ref", speed);
		                speed = -speed_ref;
		                begin_acc =  false;
		            }
		        }
		    }

		    if(begin_dec){
		        //printf("%s\n", "begin dec");
		        //printf("%s %f\n", "speed at begin of dec", speed);
		        movement = true;
		        if(forward_dir){
		            //printf("%s\n", "forward");
		            if(speed > 0){
                        //printf("%s %f\n", "speed > 0", speed);
		                speed -= dec_movement_factor;
		            }
		            else if(speed <= 0){
		            	//printf("%s\n", "speed under tres");
                        //printf("%s %f\n", "speed <= 0", speed);
		                speed = 0;
		                movement = false;
		                begin_dec = false;
		            }
		        }
		        else if(backward_dir){
		            if(speed < 0){
                        //printf("%s %f\n", "speed < 0", speed);

		                speed += dec_movement_factor;
		            }
		            else if(speed >= 0){
                        //printf("%s %f\n", "speed >= 0", speed);
		                speed = 0;
		                movement = false;
		                begin_dec =  false;
		            }
		        }
		    }

		    //printf("%s %f\n","   after",  speed);


		    vec3 translate = vec3(0.0f, 0.0f, 0.0f);
		    
		    mat4 final_matrix = IDENTITY_MATRIX;

		    if (movement){
		        translate = -cam_look*speed*0.05f;
		    }

		    cam_pos = cam_pos+translate;

		    if(this_mode == first_person){
		    	if(cam_pos.x <= -1){
		    		cam_pos.x = -1;
		    	}
		    	else if (cam_pos.x >= 5){
		    		cam_pos.x = 5;
		    	}
		    	if(cam_pos.z <= -5){
		    		cam_pos.z = -5;
		    	}
		    	else if(cam_pos.z >= 1){
		    		cam_pos.z = 1;
		    	}
		    }

		    if(rotate_factor != 0.0){
		        float x = cos(rotate_factor);
		        float y = sin(rotate_factor);
		        float a = cam_pos.x;
		        float b = cam_pos.y;
		        float c = cam_pos.z;
		        vec3 axis_norm = normalize(rotation_axis);
		        float u = axis_norm.x;
		        float v = axis_norm.y;
		        float w = axis_norm.z;

		        final_matrix[0] = vec4(pow(u,2)+(pow(v,2)+pow(w,2))*x, u*v*(1-x)+w*y, u*w*(1-x)-v*y, 0.0);
		        final_matrix[1] = vec4(u*v*(1-x)-w*y, pow(v,2)+(pow(u,2)+pow(w,2))*x, v*w*(1-x)+u*y, 0.0);
		        final_matrix[2] = vec4(u*w*(1-x)+v*y, v*w*(1-x)-u*y, pow(w,2)+(pow(u,2)+pow(v,2))*x, 0.0);
		        final_matrix[3] = vec4((a*(pow(v,2)+pow(w,2))-u*(b*v+c*w))*(1-x)+(b*w-c*v)*y, (b*(pow(u,2)+pow(w,2))-v*(a*u+c*w))*(1-x)+(c*u-a*w)*y, (c*(pow(u,2)+pow(v,2))-w*(a*u+b*v))*(1-x)+(a*v-b*u)*y, 1.0); 

		        vec4 l = final_matrix*vec4(cam_look, 0.0f);
		        vec4 n = final_matrix*vec4(cam_up, 0.0f);
		        vec4 r = final_matrix*vec4(cam_right, 0.0f);
		        
		        //cam_up = normalize(vec3(n.x, n.y, n.z));
		        cam_look = normalize(vec3(l.x, l.y, l.z));
		        cam_right = normalize(vec3(r.x, r.y, r.z));

		        rotate_factor = 0.0f;
		        rotation_axis = vec3(0.0f, 0.0f, 0.0f);
		    }
		    /*printf("%s %f %f %f\n", "position = ", cam_pos.x, cam_pos.y, cam_pos.z);
		    printf("%s %f %f %f\n", "cam look = ", cam_look.x, cam_look.y, cam_look.z);
		    printf("%s %f %f %f\n", "cam up = ", cam_up.x, cam_up.y, cam_up.z);
		    printf("%s %f %f %f\n", "cam right = ", cam_right.x, cam_right.y, cam_right.z);

		    printf("%s\n", "---------------------------");  */
		    //printf("%f %f\n", current_grid.x, current_grid.y);
		    if(this_mode == first_person){
		    	cam_pos = vec3(cam_pos.x, depth_first_person, cam_pos.z);
		    }

		    view_matrix = lookAt(cam_pos, cam_pos-cam_look, cam_up);
		    return view_matrix;
		}

		mat4 set_first_person_depth(float depth){
			depth_first_person = depth;
		}

		vec3 frame_index_and_off(){
			int x = int((cam_pos.x+1)/2.0);
			int z = int((-cam_pos.z+1)/2.0);
			//printf("%s %i %s %i\n", "x :", x, "z : ", z);
			float x_off = cam_pos.x - 2*x;
			float z_off = cam_pos.z + 2*z;
			//printf("%s %f %s %f\n", "x_off :", x_off, "z_off : ", z_off);
			int current_grid = x*3 + z;
			return vec3(current_grid, x_off, z_off); // we return the current grid and the offsets in the same vec3
		}

		void update_by_key(int key, int action){
			//printf("%i\n", key);
			if(action == GLFW_PRESS){
                //printf("%s\n","key press" );
       			keys[key] = true;
		        if(key == GLFW_KEY_W){
		            forward_dir = true;
		            backward_dir = false;
		            begin_acc = true;
		            begin_dec = false;
		            //speed = 0;
		        }
		        else if(key == GLFW_KEY_S){
		            backward_dir = true;
		            forward_dir = false;
		            begin_dec = false;
		            begin_acc = true;
		            //speed = 0;
		        }
		    }
		    else if(action == GLFW_RELEASE){
                //printf("%s\n","key release" );

		        if(key == GLFW_KEY_W && begin_dec == false){
		            forward_dir = true;
		            backward_dir = false;
		            begin_acc = false;
		            begin_dec = true;
		        }
		        else if(key == GLFW_KEY_S && begin_dec == false){
		            backward_dir = true;
		            forward_dir = false;
		            begin_dec = true;
		            begin_acc = false;
		        }
		        keys[key] = false;
		    }
		    else if (keys[GLFW_KEY_A] == true) {
		        rotate_factor = factor;
		        rotation_axis = cam_up;
		    } 
		    else if (keys[GLFW_KEY_D] == true){
		        rotate_factor = -factor;
		        rotation_axis = cam_up;
		    }  
		    else if (keys[GLFW_KEY_Q] == true) {
		        rotate_factor = -factor;
		        rotation_axis = cam_right;
		    }  
		    else if (keys[GLFW_KEY_E] == true) {
		        rotate_factor = factor;
		        rotation_axis = cam_right;
		    }   
		        
		    /*else if ((key == GLFW_KEY_P || key == GLFW_KEY_O) && action == GLFW_PRESS){
		        
		        if(key==GLFW_KEY_P){
		            polygon = true;
		        }
		        else if(key==GLFW_KEY_O){
		            polygon = false;
		        }
		    }*/
		}

		void generic_bezier(float t, int order){
			float x_coord = 0.0f;
			float z_coord = 0.0f;
			float y_coord = 0.0f;
			for (int i = 0; i <= order; ++i){
				int off = offset_bin_coeff[order];
				x_coord += bin_coeff[off+i]*pow(1-t, order-i)*pow(t, i)*bezier_control[off+i].x;
				y_coord += bin_coeff[off+i]*pow(1-t, order-i)*pow(t, i)*bezier_control[off+i].y;
				z_coord += bin_coeff[off+i]*pow(1-t, order-i)*pow(t, i)*bezier_control[off+i].z;
			}
			cam_pos = vec3(x_coord, y_coord, z_coord);
            cam_look = -vec3(sin(M_PI*t), -0.1*(1-t), cos(M_PI+M_PI*t));
            cam_right = normalize(cross(cam_up, cam_look));

        }

        /*void bezier_derivative(float n){

            for(int i = 0; i <= n, ++i){

            }
        }*/

		vec3 get_cam_pos(){
			return cam_pos;
		}

        vec3 get_cam_look() {
            return cam_look;
        }

        vec3 get_cam_up() {
            return cam_up;
        }
	
};
