#version 330 core

in struct fragment_data
{
    vec4 position;
    vec4 normal;
    vec4 color;
    vec2 texture_uv;
} fragment;

uniform float time;
uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D iChannel2;


out vec4 FragColor;

float map(vec3 p) {
	float t = time / 3.;
        p.z += t * 2.; p.x += t * 2.;
        vec3 c1 = texture(iChannel2, p.xz/15.).xyz;
  /*      p.z += t * 3.; p.x += t * 0.52;
        vec3 c2 = texture(iChannel2, p.xz/15. ).xyz;
        p.z += t * 4.; p.x += t * 0.8;
        vec3 c3 = texture(iChannel2, p.xz/15.).xyz;
        c1 += c2 - c3; */
        float z = (c1.x + c1.y + c1.z) / 8.;
        return p.y + z;
}



float intersect(vec3 ro, vec3 rd) {
	float d = 0.0;
	for (int i = 0; i <= 100; i++) {
		float h = map(ro + rd * d);
		if (h < 0.1) return  d;
		d += h;
	}
	return 0.0;
}

vec3 norm(vec3 p) {
	float eps = .1;
	return normalize(vec3(
		map(p + vec3(eps, 0, 0)) - map(p + vec3(-eps, 0, 0)),
		map(p + vec3(0, eps, 0)) - map(p + vec3(0, -eps, 0)),
		map(p + vec3(0, 0, eps)) - map(p + vec3(0, 0, -eps))
	));
}

void main() {

	vec2 uv = fragment.texture_uv - 0.5f;
	
//	vec3 l1 = normalize(vec3(1, 1, 1));
        vec3 ro = vec3(-3, 80, -3);
        vec3 rc = vec3(0, 0, 0);
	vec3 ww = normalize(rc - ro);
	vec3 uu = normalize(cross(vec3(0,1,0), ww));
	vec3 vv = normalize(cross(rc - ro, uu));
	vec3 rd = normalize(uu * uv.x + vv * uv.y + ww);
	float d = intersect(ro, rd);
	vec3 c = vec3(0.0);
	if (d > 0.0) {
		vec3 p = ro + rd * d;
		vec3 n = norm(p);
 //               float spc = pow(max(0.0, dot(reflect(l1, n), rd)), 30.0);
                vec4 ref = texture(iChannel0, normalize(reflect(rd, n)).xy);
                vec3 rfa = 10.*texture(iChannel1, (p+n).xz / 6.0).xyz * (8./d);
		//c = c+ spc;
                c = rfa.xyz + (ref.xyz * 0.5) + vec3(0.f,0.f,0.0f); // + 10*spc
	}
        FragColor = vec4((c - 0.4) * 1.1 + 0.5, 0.8 );
}
