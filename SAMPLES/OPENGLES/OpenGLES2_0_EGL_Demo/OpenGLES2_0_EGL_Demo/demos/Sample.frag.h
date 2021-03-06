#ifndef _SampleFrag_h_
#define _SampleFrag_h_


// Header generated from binary by WriteAsBinHeader()..
static const int SampleFragLength = 45;
static const unsigned int SampleFrag[SampleFragLength]={
    0x20205350,    0xFFFF0008,    0x00000048,    0x01020000,    0x00000003,    0x00000002,    0x00000000,    0x00000000,    0x00000001,    0x00000000,
    0x00000000,    0x00000000,    0x00000000,    0x00000000,    0x00000000,    0x00000000,    0x00000005,    0x00000000,    0x00000000,    0x00000000,
    0x00B820A4,    0x00000000,    0x00000000,    0x01000000,    0x00FA10E4,    0x00000000,    0x00000000,    0x00000000,    0x1E000000,    0x00000000,
    0x00000000,    0x00000000,    0x00000000,    0x00000000,    0x3F800000,    0x3F800000,    0x3F800000,    0x3F800000,    0x00000000,    0x00000004,
    0x00000006,    0x00000000,    0x00000000,    0x736F506E,    0x00000000,};

//checksum generated by simpleCheckSum()
static const unsigned int SampleFragCheckSum = 72;

static const char* SampleFragText = 
    "\n"
    "#ifdef GL_ES\n"
    "precision highp float;\n"
    "#endif\n"
    "\n"
    "varying vec3 nPos;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor.xyz = nPos;\n"
    "}";

#ifdef GL_HELPERS_INCLUDED
//glHelpers.h must be included BEFORE any of the shader header files. Also make sure you have the latest version of glHelpers.h
static ghShader SampleFragShader(SampleFragText, SampleFrag, SampleFragLength, SampleFragCheckSum);


#endif


#endif //_SampleFrag_h_
