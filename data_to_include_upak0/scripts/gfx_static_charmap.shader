// Mix3r_Durachok: shaders for charmap support are here
// new one overrides native shaders
gfx/2d/bigchar2
{
        nopicmip
        nomipmaps
        {
                map gfx/2d/charmap_ru.tga
                blendfunc blend
                rgbgen vertex
        }
}
// old one for compatibility with engines other than q3e-based
gfx/2d/bigchars
{
        nopicmip
        nomipmaps
        {
                map gfx/2d/charmap_ru.tga
                blendfunc blend
                rgbgen vertex
        }
}
// basic color non-texture shaders:
textures/common/a_black
{
	surfaceparm nolightmap
        {
                map $whiteimage
                rgbGen const ( 0.0 0.0 0.0 )
        }
}
textures/common/a_gray
{
	surfaceparm nolightmap
        {
                map $whiteimage
                rgbGen const ( 0.25 0.25 0.25 )
        }
}
textures/common/a_white
{
	surfaceparm nolightmap
        {
                map $whiteimage
                rgbGen const ( 0.5 0.5 0.5 )
        }
}
textures/common/a_white_lm
{
        {
                map $whiteimage
                rgbGen const ( 1.0 1.0 1.0 )
        }
        {
                map $lightmap
                rgbGen identity
                blendfunc filter
        }
}
textures/common/a_gray_lm
{
        {
                map $whiteimage
                rgbGen const ( 0.5 0.5 0.5 )
        }
        {
                map $lightmap
                rgbGen identity
                blendfunc filter
        }
}
textures/common/a_green
{
	surfaceparm nolightmap
        {
                map $whiteimage
                rgbGen const ( 0.0 0.5 0.0 )
        }
}
textures/common/a_green_dark
{
	surfaceparm nolightmap
        {
                map $whiteimage
                rgbGen const ( 0.0 0.25 0.0 )
        }
}
textures/common/a_green_lm
{
        {
                map $whiteimage
                rgbGen const ( 0.0 1.0 0.0 )
        }
        {
                map $lightmap
                rgbGen identity
                blendfunc filter
        }
}
textures/common/a_green_dark_lm
{
        {
                map $whiteimage
                rgbGen const ( 0.0 0.5 0.0 )
        }
        {
                map $lightmap
                rgbGen identity
                blendfunc filter
        }
}
textures/common/a_orange_lm
{
        {
                map $whiteimage
                rgbGen const ( 1.0 0.5 0.0 )
        }
        {
                map $lightmap
                rgbGen identity
                blendfunc filter
        }
}
textures/common/a_orange_dark_lm
{
        {
                map $whiteimage
                rgbGen const ( 0.5 0.25 0.0 )
        }
        {
                map $lightmap
                rgbGen identity
                blendfunc filter
        }
}
textures/common/a_yellow_lm
{
        {
                map $whiteimage
                rgbGen const ( 1.0 1.0 0.0 )
        }
        {
                map $lightmap
                rgbGen identity
                blendfunc filter
        }
}
textures/common/a_yellow_dark_lm
{
        {
                map $whiteimage
                rgbGen const ( 0.5 0.5 0.0 )
        }
        {
                map $lightmap
                rgbGen identity
                blendfunc filter
        }
}
textures/common/a_red_lm
{
        {
                map $whiteimage
                rgbGen const ( 1.0 0.0 0.0 )
        }
        {
                map $lightmap
                rgbGen identity
                blendfunc filter
        }
}
textures/common/a_red_dark_lm
{
        {
                map $whiteimage
                rgbGen const ( 0.5 0.0 0.0 )
        }
        {
                map $lightmap
                rgbGen identity
                blendfunc filter
        }
}
textures/common/a_blue_lm
{
        {
                map $whiteimage
                rgbGen const ( 0.0 0.0 1.0 )
        }
        {
                map $lightmap
                rgbGen identity
                blendfunc filter
        }
}
textures/common/a_blue_dark_lm
{
        {
                map $whiteimage
                rgbGen const ( 0.0 0.0 0.5 )
        }
        {
                map $lightmap
                rgbGen identity
                blendfunc filter
        }
}
textures/common/a_magenta_lm
{
        {
                map $whiteimage
                rgbGen const ( 1.0 0.0 1.0 )
        }
        {
                map $lightmap
                rgbGen identity
                blendfunc filter
        }
}
textures/common/a_magenta_dark_lm
{
        {
                map $whiteimage
                rgbGen const ( 0.5 0.0 0.5 )
        }
        {
                map $lightmap
                rgbGen identity
                blendfunc filter
        }
}
textures/common/a_turquoise_lm
{
        {
                map $whiteimage
                rgbGen const ( 0.0 1.0 1.0 )
        }
        {
                map $lightmap
                rgbGen identity
                blendfunc filter
        }
}
textures/common/a_turquoise_dark_lm
{
        {
                map $whiteimage
                rgbGen const ( 0.0 0.5 0.5 )
        }
        {
                map $lightmap
                rgbGen identity
                blendfunc filter
        }
}