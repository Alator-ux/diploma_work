#version 410
// Code adapted from
// http://horde3d.org/wiki/index.php5?title=Shading_Technique_-_FXAA

in vec2 TexCoords;
uniform sampler2D tex;
uniform vec2 texCoordsOffset;
uniform float SpanMax;
uniform float ReduceMin;
uniform float ReduceMul;

out vec4 outColor;

void main()
{
	// Существует подход, использующий только rg, т.к. в играх почти нет синего, 
	// однако он здесь не подходит
	const vec3 lum = vec3(0.299, 0.587, 0.114);	

    // Получение пикселей соседних и рассматриваемого
    // Сверху слева
	vec3 pixelNW = texture2D(tex, TexCoords.xy + (vec2(-1.0, -1.0) * texCoordsOffset)).rgb;
    // Сверху справа
    vec3 pixelNE = texture2D(tex, TexCoords.xy + (vec2(1.0, -1.0) * texCoordsOffset)).rgb;
    // Снизу справа
    vec3 pixelSE = texture2D(tex, TexCoords.xy + (vec2(1.0, 1.0) * texCoordsOffset)).rgb;
    // Снизу слева
    vec3 pixelSW = texture2D(tex, TexCoords.xy + (vec2(-1.0, 1.0) * texCoordsOffset)).rgb;
    // Рассматрвиаемый пиксель
	vec3 pixelC  = texture2D(tex, TexCoords.xy).rgb;
    
    // Вычисление яркости рассматриваемых пикселей.
    float lumNW = dot(pixelNW, lum);
    float lumNE = dot(pixelNE, lum);
    float lumSE = dot(pixelSE, lum);
    float lumSW = dot(pixelSW, lum);
    float lumC  = dot(pixelC, lum);

    // Вычисление минимальной
    float lumMin = min(lumC, min(lumNW, min(lumNE, min(lumSE, lumSW))));
    // и максимальной
    float lumMax = max(lumC, max(lumNW, max(lumNE, max(lumSE, lumSW))));

	vec2 dir = vec2(-((lumNW + lumNE) - (lumSW + lumSE)),
					((lumNW + lumSW) - (lumNE + lumSE)));
	
	// Коэффициент уменьшения направления, чтобы рассматривать необходимые пиксели.
	// Умножение на 0.25 для вычисления средней яркости
	float dirReduce = max(
        (lumNW + lumNE + lumSW + lumSE) * (0.25 * ReduceMul), 
        ReduceMin);
	// добавление dirReduce, чтобы исключить ситуацию деления на ноль.
	float inverseDirAdjustment = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);
	
	dir = clamp(dir * inverseDirAdjustment, vec2(-SpanMax), vec2(SpanMax)) * texCoordsOffset;

	// У каждого пикселя вес 1/2
	vec3 pRes1 = (0.5) * (
		texture2D(tex, TexCoords.st + (dir * vec2(1.0/3.0 - 0.5))).rgb +
		texture2D(tex, TexCoords.st + (dir * vec2(2.0/3.0 - 0.5))).rgb);

	// Можем попытаться взять еще больше пикселей. У каждого пикселя вес 1/4.
	vec3 pRes2 = pRes1 * (0.5) + (0.25) * (
		texture2D(tex, TexCoords.st + (dir * vec2(0.0/3.0 - 0.5))).rgb +
		texture2D(tex, TexCoords.st + (dir * vec2(3.0/3.0 - 0.5))).rgb);


	float lumTest = dot(lum, pRes2);
	// Если вышли за границы допустимой яркости, то для размытия берем более меньший квадрат
	if(lumTest < lumMin || lumTest > lumMax)
		outColor = vec4(pRes1, 1.0);
	else
		// Иначе больший
		outColor = vec4(pRes2, 1.0);
}