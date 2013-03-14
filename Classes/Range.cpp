#include "Range.h"
const char *vert = 
"													\n\
attribute vec4 a_position;							\n\
attribute vec2 a_texCoord;							\n\
attribute vec4 a_color;								\n\
													\n\
#ifdef GL_ES										\n\
varying lowp vec4 v_fragmentColor;					\n\
varying mediump vec2 v_texCoord;					\n\
#else												\n\
varying vec4 v_fragmentColor;						\n\
varying vec2 v_texCoord;							\n\
#endif												\n\
varying vec2 pos;                                   \n\
													\n\
void main()											\n\
{													\n\
    pos = a_position.xy;                            \n\
    gl_Position = CC_MVPMatrix * a_position;		\n\
	v_fragmentColor = a_color;						\n\
	v_texCoord = a_texCoord;						\n\
}													\n\
";

const char *frag = 
"#ifdef GL_ES								\n"
"precision lowp float;						\n"
"#endif										\n"
"											\n"
"varying vec4 v_fragmentColor;				\n"
"varying vec2 v_texCoord;					\n"
"uniform sampler2D CC_Texture0;				\n"
"uniform float radius;                       \n"
"varying vec2 pos;                           \n"
"											\n"
"void main()									\n"
"{											\n"
"    if(length(pos) < radius) {               \n"
"       if(length(pos) > radius*0.9) {\n"
"          gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n "
"       }\n"
"       else {\n"
    "       float density  = 1.-exp(-length(pos)/radius);\n"
    "       gl_FragColor = vec4(density, density, density, density);\n"
"       }"
"    } else                                    \n"
"        discard; \n"
"}	                                        \n"										
;



Range *Range::create() {
    Range *pRet = new Range();
    pRet->init();
    pRet->autorelease();
    return pRet;
}
void Range::init() {
    CCGLProgram *p = new CCGLProgram();
    p->initWithVertexShaderByteArray(vert, frag);
    
    p->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
    p->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
    p->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);

    p->link();
    p->updateUniforms();
    
    CHECK_GL_ERROR_DEBUG();

    CCShaderCache::sharedShaderCache()->addProgram(p, "rangeShader");
    p->release();

    CCGLProgram *temp = CCShaderCache::sharedShaderCache()->programForKey("rangeShader");
    CCLOG("shader is %x %x", p, temp);
    

    
    setShaderProgram(temp);

    sy = 0.8;
    radius = 100;

    setScaleY(sy);
    setPosition(ccp(300, 300));

    GLuint radLoc = glGetUniformLocation(temp->getProgram(), "radius");
    temp->setUniformLocationWith1f(radLoc, radius);

}

void Range::draw() {
    CC_NODE_DRAW_SETUP();

    CCPoint origin = ccp(-radius*1.5, -radius*1.5);
    CCPoint destination = ccp(radius*1.5, radius*1.5);
    CCPoint vertices[] = {
        origin,
        ccp(destination.x, origin.y),
        destination,
        ccp(origin.x, destination.y)
    };

    unsigned int numberOfPoints = 4;
    ccVertex2F* newPoli = new ccVertex2F[numberOfPoints]; //4个点

    for( unsigned int i=0; i<numberOfPoints;i++)
    {
        newPoli[i] = vertex2( vertices[i].x, vertices[i].y );
    }
    glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, newPoli);

    glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei) numberOfPoints);

    CC_SAFE_DELETE_ARRAY(newPoli);
    CC_INCREMENT_GL_DRAWS(1);
}
