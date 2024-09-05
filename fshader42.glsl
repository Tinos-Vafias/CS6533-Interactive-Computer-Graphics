/*****************************
 * File: fshader42.glsl
 *       A simple fragment shader
 *****************************/

#version 150  // YJC: Comment/un-comment this line to resolve compilation errors
              //      due to different settings of the default GLSL version

in  vec4 color;
in float z; 
in float fog_set;
in  vec2 texCoord;
in float texCoordStripe;
in vec2 latticeCoord;
out vec4 fColor;

// defined variables for calculating fog
vec4 fog        = vec4(0.7, 0.7, 0.7, 0.5);
float start     = 0.0;  
float end       = 18.0;
float density   = 0.09;
float fog_factor;

uniform sampler2D texture_2D; /* Note: If using multiple textures,
                                       each texture must be bound to a
                                       *different texture unit*, with the
                                       sampler uniform var set accordingly.
                                 The (fragment) shader can access *all texture units*
                                 simultaneously.
                              */
uniform sampler1D texture_1D;

uniform int textureFlag; // 0: no texture application: obj color
                              // 1: texutre color
                              // 2: (obj color) * (texture color)
uniform bool draw_floor;
uniform bool draw_sphere;
uniform bool draw_sp_tex;
uniform int draw_sp_ver;
uniform bool lattice_shadow;
uniform bool lattice;

void main() 
{ 

    if (fog_set == 0){
        fColor = color;
    } else  if (fog_set == 1) {
        fog_factor = (end - z) / (end - start);
        fog_factor = clamp(fog_factor, 0, 1);
        fColor = fog_factor * color + (1 - fog_factor) * fog;
    } else if (fog_set == 2) {
        fog_factor = exp(- (density * z));
        fog_factor = clamp(fog_factor, 0, 1);
        fColor = fog_factor * color + (1 - fog_factor) * fog;
    } else if (fog_set == 3) {
        fog_factor = exp(- ((density * z) * (density * z)));
        fog_factor = clamp(fog_factor, 0, 1);
        fColor = fog_factor * color + (1 - fog_factor) * fog;
    }

    if (draw_floor){
        if (textureFlag == 0){
            fColor = color;
        }
        else if (textureFlag == 1){
            fColor = color * texture( texture_2D, texCoord ); 
        }
    }

    float frac_s = fract(4.0 * latticeCoord.s);
    float frac_t = fract(4.0 * latticeCoord.t);
    if (draw_sphere && draw_sp_tex){
        if (draw_sp_ver == 0){
            fColor = color * texture(texture_1D, texCoordStripe);
        } else {
            vec4 checker = texture(texture_2D, texCoord);
            if (checker.r == 0.0 && checker.g > 0.0 && checker.b == 0.0){
                fColor = color * vec4(0.9, 0.1, 0.1, 1.0);
            } else {
                fColor = color * texture(texture_2D, texCoord);
            }        
        }

        if (frac_s < .35 && frac_t < .35 && lattice){
            discard;
        }
    } else if (draw_sphere && lattice) {
            if (frac_s < .35 && frac_t < .35 && lattice){
            discard;
        }
    }
    if (lattice_shadow && lattice){
            if (frac_s < .35 && frac_t < .35){
            discard;
        }
    }

        if (fog_set == 0){
        fColor = fColor;
    } else  if (fog_set == 1) {
        fog_factor = (end - z) / (end - start);
        fog_factor = clamp(fog_factor, 0, 1);
        fColor = fog_factor * fColor + (1 - fog_factor) * fog;
    } else if (fog_set == 2) {
        fog_factor = exp(- (density * z));
        fog_factor = clamp(fog_factor, 0, 1);
        fColor = fog_factor * fColor + (1 - fog_factor) * fog;
    } else if (fog_set == 3) {
        fog_factor = exp(- ((density * z) * (density * z)));
        fog_factor = clamp(fog_factor, 0, 1);
        fColor = fog_factor * fColor + (1 - fog_factor) * fog;
    }
} 

