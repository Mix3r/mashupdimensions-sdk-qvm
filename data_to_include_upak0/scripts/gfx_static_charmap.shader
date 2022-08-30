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
