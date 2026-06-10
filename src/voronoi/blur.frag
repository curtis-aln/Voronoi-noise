uniform sampler2D texture;
uniform vec2 texelSize;
uniform float blur_radius;

void main()
{
    vec2 uv = gl_TexCoord[0].xy;

    // 5x5 gaussian — wider spread for organic cell edges
    vec4 col = vec4(0.0);
    float total = 0.0;

    for (int x = -2; x <= 2; x++)
    {
        for (int y = -2; y <= 2; y++)
        {
            // gaussian weight: e^(-(x^2 + y^2) / (2 * sigma^2))
            float sigma = 1.5;
            float weight = exp(-float(x*x + y*y) / (2.0 * sigma * sigma));

            vec2 offset = vec2(float(x), float(y)) * texelSize * blur_radius;
            col += texture2D(texture, uv + offset) * weight;
            total += weight;
        }
    }

    gl_FragColor = gl_Color * (col / total);
}