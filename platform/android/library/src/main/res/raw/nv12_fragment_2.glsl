#version 100
precision mediump float;

varying vec2 v_coordinate;

uniform sampler2D y_tex;
uniform sampler2D uv_tex;


void main()
{

    // 获取 Y 分量
    float y = texture2D(y_tex, v_coordinate).r;

    // 获取 UV 分量
    vec2 uv = texture2D(uv_tex, v_coordinate).ra;

    // 解码 UV 到 U 和 V
    float u = uv.r - 0.5;
    float v = uv.g - 0.5;

    // YUV 转 RGB
    float r = y + 1.402 * v;
    float g = y - 0.344 * u - 0.714 * v;
    float b = y + 1.772 * u;

    gl_FragColor = vec4(r, g, b, 1.0);
}
