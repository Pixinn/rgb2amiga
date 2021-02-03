set(ImageMagick_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/ImageMagick ${CMAKE_CURRENT_LIST_DIR}/ImageMagick/Magick++/lib)

set(ImageMagickLibDir ${CMAKE_CURRENT_LIST_DIR}/VisualMagick/lib)

set(MAGIC_LIB  "$<$<CONFIG:debug>:${ImageMagickLibDir}/CORE_DB_Magick_.lib>$<$<CONFIG:release>:${ImageMagickLibDir}/CORE_RL_Magick_.lib>")
set(MAGICKPP_LIB  "$<$<CONFIG:debug>:${ImageMagickLibDir}/CORE_DB_Magick++_.lib>$<$<CONFIG:release>:${ImageMagickLibDir}/CORE_RL_Magick++_.lib>")
set(WAND_LIB  "$<$<CONFIG:debug>:${ImageMagickLibDir}/CORE_DB_wand_.lib>$<$<CONFIG:release>:${ImageMagickLibDir}/CORE_RL_wand_.lib>")

set(ImageMagick_LIBRARIES ${MAGIC_LIB} ${MAGICKPP_LIB} ${WAND_LIB})
