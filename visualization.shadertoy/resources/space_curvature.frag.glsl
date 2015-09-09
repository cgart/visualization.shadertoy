// see https://www.shadertoy.com/view/llj3Rz#

// Created by inigo quilez - iq/2015
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

// modified slightly by cgart

vec3 fancyCube( sampler2D sam, in vec3 d, in float s, in float b )
{
    vec3 colx = texture2D( sam, 0.5 + s*d.yz/d.x, b ).xyz;
    vec3 coly = texture2D( sam, 0.5 + s*d.zx/d.y, b ).xyz;
    vec3 colz = texture2D( sam, 0.5 + s*d.xy/d.z, b ).xyz;

    vec3 n = d*d;

    return (colx*n.x + coly*n.y + colz*n.z)/(n.x+n.y+n.z);
}


vec2 hash( vec2 p ) { p=vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))); return fract(sin(p)*43758.5453); }

vec2 voronoi( in vec2 x )
{
    vec2 n = floor( x );
    vec2 f = fract( x );

	vec3 m = vec3( 8.0 );
    for( int j=-1; j<=1; j++ )
    for( int i=-1; i<=1; i++ )
    {
        vec2  g = vec2( float(i), float(j) );
        vec2  o = hash( n + g );
        vec2  r = g - f + o;
		float d = dot( r, r );
        if( d<m.x )
            m = vec3( d, o );
    }

    return vec2( sqrt(m.x), m.y+m.z );
}

float shpIntersect( in vec3 ro, in vec3 rd, in vec4 sph )
{
    vec3 po = ro - sph.xyz;

    float b = dot( rd, po );
    float c = dot( po, po ) - sph.w*sph.w;
    float h = b*b - c;

    if( h>0.0 ) h = -b - sqrt( h );

    return h;
}

float sphDistance( in vec3 ro, in vec3 rd, in vec4 sph )
{
	vec3 oc = ro - sph.xyz;
    float b = dot( oc, rd );
    float c = dot( oc, oc ) - sph.w*sph.w;
    float h = b*b - c;
    float d = sqrt( max(0.0,sph.w*sph.w-h)) - sph.w;
    return d;
}

float sphSoftShadow( in vec3 ro, in vec3 rd, in vec4 sph, in float k )
{
    vec3 oc = sph.xyz - ro;
    float b = dot( oc, rd );
    float c = dot( oc, oc ) - sph.w*sph.w;
    float h = b*b - c;
    return (b<0.0) ? 1.0 : 1.0 - smoothstep( 0.0, 1.0, k*h/b );
}


vec3 sphNormal( in vec3 pos, in vec4 sph )
{
    return (pos - sph.xyz)/sph.w;
}

//=======================================================

vec3 background( in vec3 d, in vec3 l )
{
    vec3 col = vec3(0.0);
         col += 0.5*pow( fancyCube( iChannel1, d, 0.05, 5.0 ).zyx, vec3(2.0) );
         col += 0.2*pow( fancyCube( iChannel1, d, 0.10, 3.0 ).zyx, vec3(1.5) );
         col += 0.8*vec3(0.80,0.5,0.6)*pow( fancyCube( iChannel1, d, 0.1, 0.0 ).xxx, vec3(6.0) );
    float stars = smoothstep( 0.3, 0.7, fancyCube( iChannel1, d, 0.91, 0.0 ).x );


    vec3 n = abs(d);
    n = n*n*n;
    vec2 vxy = voronoi( 50.0*d.xy );
    vec2 vyz = voronoi( 50.0*d.yz );
    vec2 vzx = voronoi( 50.0*d.zx );
    vec2 r = (vyz*n.x + vzx*n.y + vxy*n.z) / (n.x+n.y+n.z);
    col += 0.9 * stars * clamp(1.0-(3.0+r.y*5.0)*r.x,0.0,1.0);

    col = 1.5*col - 0.2;
    col += vec3(-0.05,0.1,0.0);

    float s = clamp( dot(d,l), 0.0, 1.0 );
    col += 0.4*pow(s,5.0)*vec3(1.0,0.7,0.6)*2.0;
    col += 0.4*pow(s,64.0)*vec3(1.0,0.9,0.8)*2.0;

    float freq = texture2D(iChannel0, vec2(0.05, 0.25)).x;
    col.r += (0.25 * pow(freq, 0.5));

    return col;

}


//--------------------------------------------------------------------

vec4 sph1 = vec4( 0.0, 0.0, 0.0, 1.0 );

float rayTrace( in vec3 ro, in vec3 rd )
{
    return shpIntersect( ro, rd, sph1 );
}

float map( in vec3 pos )
{
    float d = length( pos.xz - sph1.xz );

    float freq = texture2D(iChannel0, vec2(d * 0.2, 0.25)).x;


    float h = 1.0-2.0/(1.0 + 0.3*d*d);

    return pos.y - h * (1.0 + freq * 0.35);
}

float rayMarch( in vec3 ro, in vec3 rd, float tmax )
{
    float t = 0.0;

    // bounding plane
    float h = (1.0-ro.y)/rd.y;
    if( h>0.0 ) t=h;

    // raymarch 30 steps
    for( int i=0; i<10; i++ )
    {
        vec3 pos = ro + t*rd;
        float h = map( pos );
        if( h<0.001 || t>tmax ) break;
        t += h;
    }
    return t;
}

vec3 render( in vec3 ro, in vec3 rd )
{
    vec3 lig = normalize( vec3(1.0,0.2,1.0) );
    vec3 col = background( rd, lig );

    // raytrace stuff
    float t = rayTrace( ro, rd );

    //vec4 freqs = sampleSound();

    if( t>0.0 )
    {
        col = vec3(0.,0.,0.);
    }

    // raymarch stuff
    float tmax = 10.0;
    if( t>0.0 ) tmax = t;
    t = rayMarch( ro, rd, tmax );
    if( t<tmax )
    {
            vec3 pos = ro + t*rd;

            vec2 scp = sin(3.0*6.2831*pos.xz);

            vec3 wir = vec3( 0.0 );
            wir += 1.0*exp(-12.0*abs(scp.x));
            wir += 1.0*exp(-12.0*abs(scp.y));
            wir += 0.5*exp( -4.0*abs(scp.x));
            wir += 0.5*exp( -4.0*abs(scp.y));
            //wir *= 0.2 + 1.0*sphSoftShadow( pos, lig, sph1, 4.0 );

            col += wir*0.5*exp( -0.05*t*t );;
    }

    float sndbass = texture2D(iChannel0, vec2(0.01, 0.25)).x;

    if( dot(rd,sph1.xyz-ro)>0.0 )
    {
        float d = sphDistance( ro, rd, sph1 );
        vec3 glo = vec3(0.0);
        glo += vec3(0.6,0.7,1.0)*0.3*exp(-2.0*abs(d))*step(0.0,d);
        glo += 0.6*vec3(0.6,0.7,1.0)*0.3*exp(-8.0*abs(d));
        glo += 0.6*vec3(0.8,0.9,1.0)*0.4*exp(-100.0*abs(d));
        col += glo*2.0 * (1.0 + pow(sndbass, 5.));
    }

    col *= smoothstep( 0.0, 6.0, iGlobalTime);

    return col;
}


mat3 setCamera( in vec3 ro, in vec3 rt, in float cr )
{
	vec3 cw = normalize(rt-ro);
	vec3 cp = vec3(sin(cr), cos(cr),0.0);
	vec3 cu = normalize( cross(cw,cp) );
	vec3 cv = normalize( cross(cu,cw) );
    return mat3( cu, cv, -cw );
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 p = (-iResolution.xy +2.0*fragCoord.xy) / iResolution.y;

    float zo = 2.0;// + smoothstep( 5.0, 15.0, abs(iGlobalTime-48.0) );
    float an = 3.0 + 0.05*iGlobalTime;// + 6.0*iMouse.x/iResolution.x;
    vec3 ro = zo*vec3( 2.0*cos(an), 1.0, 2.0*sin(an) );
    vec3 rt = vec3( 1.0, 0.0, 0.0 );
    mat3 cam = setCamera( ro, rt, 0.35 );
    vec3 rd = normalize( cam * vec3( p, -2.0) );

    vec3 col = render( ro, rd );

    vec2 q = fragCoord.xy / iResolution.xy;
    col *= 0.2 + 0.8*pow( 16.0*q.x*q.y*(1.0-q.x)*(1.0-q.y), 0.1 );

	fragColor = vec4( col, 1.0 );
}
