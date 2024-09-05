/***************************
 * File: vshader42.glsl:
 *   A simple vertex shader.
 *
 * - Vertex attributes (positions & colors) for all vertices are sent
 *   to the GPU via a vertex buffer object created in the OpenGL program.
 *
 * - This vertex shader uses the Model-View and Projection matrices passed
 *   on from the OpenGL program as uniform variables of type mat4.
 ***************************/

#version 150  // YJC: Comment/un-comment this line to resolve compilation errors
              //      due to different settings of the default GLSL version

in  vec4 vPosition;
in  vec4 vColor;
in  vec2 vTexCoord;
in  vec3 vNormal;
out vec4 color;
out float z;
out float fog_set;
out vec2 texCoord;
out float texCoordStripe;
out vec2 latticeCoord;

uniform mat4 ModelView;
uniform mat4 Projection;
uniform mat3 Normal_Matrix;

// Parameters of the material
uniform vec4 m_ambient;
uniform vec4 m_diffuse;
uniform vec4 m_specular;
uniform float m_shininess;
uniform bool m_shading;
uniform bool m_source;
uniform vec4 m_light_source;

// Parameters of global ambient light
uniform vec4 g_ambient;

// Parameters of distant(directional) light
uniform vec4 d_direction;
uniform vec4 d_ambient;
uniform vec4 d_diffuse;
uniform vec4 d_specular;

// Parameters of the positional light
uniform vec4 p_ambient;
uniform vec4 p_diffuse;
uniform vec4 p_specular;

// values of positional attenuation
uniform float const_att;  // Constant Attenuation
uniform float linear_att; // Linear Attenuation
uniform float quad_att;   // Quadratic Attenuation

// values for spotlight computation
uniform vec3 sp_direction; // Direction of the spotlight
uniform float sp_angle;    // Angle for spotlight
uniform float sp_exponent; // Exponent for spotlight

//value for fog setting
uniform float fog_setting;

// value for defining object space or eye space
uniform int space;
// value for defining vertical or slanted
uniform int mode;
uniform int upright_tilted;

uniform bool draw_sphere;
uniform bool draw_sp_tex;
float spot_attenuation;

void main()
{
    //vec4 vColor4 = vec4(vColor.r, vColor.g, vColor.b, 1.0);

    gl_Position = Projection * ModelView * vPosition;

    // Compute the component of global ambient light
    vec4 g_result = m_ambient * g_ambient;

    // Compute the component of distant(directional) light
    float attenuation = 1.0;

    vec4 d_ambient_result = m_ambient * d_ambient;

    vec3 pos = (ModelView * vPosition).xyz;
    vec3 L = normalize(-d_direction.xyz);
    vec3 E = normalize( -pos );
    vec3 H = normalize( L + E );
    vec3 N = normalize(Normal_Matrix * vNormal);

    float d = max(dot(L, N), 0.0);
    vec4 d_diffuse_result = m_diffuse * d_diffuse * d;

    float s = pow( max(dot(N, H), 0.0), m_shininess);
    vec4 d_specular_result = m_specular * d_specular * s;
    
    if( dot(L, N) < 0.0 ) {
        d_specular_result = vec4(0.0, 0.0, 0.0, 1.0);
    }
    
    vec4 d_result = attenuation * (d_ambient_result + d_diffuse_result + d_specular_result);

    // ------------------------------------------------------------
    // Compute component of point source light(part d)
    float distance_to_light = length(m_light_source - gl_Position);

    //vec4 p_result = vec4(0.0, 0.0, 0.0, 1.0); // to be computed
    vec4 p_ambient_result = m_ambient * p_ambient;

    vec3 p_pos = (ModelView * vPosition).xyz;
    vec3 p_L = normalize(m_light_source.xyz - gl_Position.xyz);
    vec3 p_E = normalize( -pos );
    vec3 p_H = normalize( p_L + p_E );
    vec3 p_N = normalize(Normal_Matrix * vNormal);

    float p_d = max(dot(p_L, p_N), 0.0);
    vec4 p_diffuse_result = m_diffuse * p_diffuse * p_d;

    float p_s = pow( max(dot(p_N, p_H), 0.0), m_shininess);
    vec4 p_specular_result = m_specular * p_specular * p_s;
    
    if( dot(p_L, p_N) < 0.0 ) {
        p_specular_result = vec4(0.0, 0.0, 0.0, 1.0);
    }

    float pos_attenuation = 1.0 / (const_att + linear_att * distance_to_light + quad_att * distance_to_light * distance_to_light);
    vec4 p_result = pos_attenuation * (p_ambient_result + p_diffuse_result + p_specular_result);

    // ------------------------------------------------------------
    // Compute Component of Spot Light (part d)

    if(dot(sp_direction.xyz, -p_L) >= cos(sp_angle)){
        spot_attenuation = pow((dot(sp_direction.xyz, -p_L)), sp_exponent);
        spot_attenuation = pos_attenuation * spot_attenuation;
    } 
    else{
        spot_attenuation = 0;
    }
    vec4 sp_result = spot_attenuation * (p_ambient_result + p_diffuse_result + p_specular_result);

    // ------------------------------------------------------------
    // Overall formula to output color to the fragment shader
    if (m_shading == true){
        if (m_source == true){
            // POINT SOURCE
           color = g_result + d_result + p_result;
        } else {
            // SPOT LIGHT
           color = g_result + d_result + sp_result;
        }
    } else {
        // LIGHTING DISABLED, AXES, SHADOW
        color = vColor;
    }
    // ------------------------------------------------------------
    // Passing finding the 'z' variable for fog computation
    vec4 eye_pos = vPosition * ModelView;
    z = length(eye_pos.xyz);
    // Passing in the fog setting to the vertex shader
    fog_set = fog_setting;
    // ------------------------------------------------------------

    // ------------------------------------------------------------
    // Calculate the stripe texture Coordinate
    if (mode == 0){ //vertical
        if (space == 0){ // eye-space
            texCoordStripe = 2.5 * pos.x;
        } else { // object-space
            texCoordStripe = 2.5 * vPosition.x;
        }
    } else { //slanted
        if (space == 0){ // eye-space
            texCoordStripe = 1.5 * (pos.x + pos.y + pos.z);
        } else { // object-space
            texCoordStripe = 1.5 * (vPosition.x + vPosition.y + vPosition.z);
        }
    }
    // ------------------------------------------------------------
    // Calculate the sphere's texture Coordinate
    if (draw_sphere){
        if (mode == 0){ //vertical
            if (space == 0){ // eye-space
                texCoord = 0.5 * vec2(pos.x + 1, pos.y + 1);
            } else { // object-space
                texCoord = 0.5 * vec2(vPosition.x + 1, vPosition.y + 1);
            }
        } else { //slanted
            if (space == 0){ // eye-space
                texCoord = 0.3 * vec2((pos.x + pos.y + pos.z), (pos.x - pos.y + pos.z));
            } else { // object-space
                texCoord = 0.3 * vec2((vPosition.x + vPosition.y + vPosition.z), (vPosition.x - vPosition.y + vPosition.z));
            }
        }
    } else {
        texCoord = vTexCoord;
    }
    // ------------------------------------------------------------
    // Calculate the lattice's texture Coordinate
    if (upright_tilted == 1){ //upright
        latticeCoord = 0.5 * vec2(vPosition.x + 1, vPosition.y + 1);
    } else { // tilted
        latticeCoord = 0.3 * vec2((vPosition.x + vPosition.y + vPosition.z), (vPosition.x - vPosition.y + vPosition.z));
    }
}

