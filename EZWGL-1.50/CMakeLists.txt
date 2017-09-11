
ADD_DEFINITIONS(-DUSE_PROTOTYPE)

IF(NOT APPLE)
  ADD_DEFINITIONS(-D_XOPEN_SOURCE=500)
ENDIF(NOT APPLE)



set(EZWGL_SRC
  lib/EZ_DnD.h             lib/EZ_MemDebug.h        lib/EZ_WidgetArrowBtn.c    lib/EZ_WidgetMBar.c
  lib/ansi_stdlib.h        lib/EZ_DnDHandler1.c     lib/EZ_Message.c         lib/EZ_WidgetAttr.h        lib/EZ_WidgetMenuItems.c
  lib/EZ_Args.h            lib/EZ_DnDMsg.c          lib/EZ_Misc.c            lib/EZ_WidgetBorder.c      lib/EZ_WidgetMisc.c
  lib/EZ_Arrow.c           lib/EZ_DpyList.c         lib/EZ_WidgetButtonC.c     lib/EZ_WidgetNB.c
  lib/EZ_BMP.c             lib/EZ_DragXpm.c         lib/EZ_PolyET.c          lib/EZ_WidgetButtonM.c     lib/EZ_WidgetNBPage.c
  lib/EZ_Buffers.c         lib/EZ_DrawSpecial.c     lib/EZ_PolyET.h          lib/EZ_WidgetButtonN.c     lib/EZ_WidgetNWLabel.c
  lib/EZ_Clear.c           lib/EZ_EmbedMsg.h        lib/EZ_Poly.h            lib/EZ_WidgetButtonR.c     lib/EZ_WidgetOptInput.c
  lib/EZ_ClearP.c          lib/EZ_Enable.c          lib/EZ_Polyhedra.c       lib/EZ_Widget.c            lib/EZ_WidgetPaneHandle.c
  lib/EZ_Color.c           lib/EZ_Focus.c           lib/EZ_ProjView.c        lib/EZ_WidgetConfig.h      lib/EZ_WidgetPopup.c
  lib/EZ_ColorDefs.h       lib/EZ_Fog.c             lib/EZ_RCGM.c            lib/EZ_WidgetConfigure.c   lib/EZ_WidgetRawXWin.c
  lib/EZ_Color.h           lib/EZ_Garbage.c         lib/EZ_Render1.h         lib/EZ_WidgetDial.c        lib/EZ_WidgetRowCol.c
  lib/EZ_Comm.c            lib/EZ_GIF.c             lib/EZ_Render.c          lib/EZ_WidgetDisp.c        lib/EZ_WidgetRuler.c
  lib/EZ_Constants.h       lib/EZ_GLEvent.c         lib/EZ_RenderFunc.h      lib/EZ_WidgetEmbeder.c     lib/EZ_WidgetScrollButton.c
  lib/EZ_ConvertLine00.h   lib/EZ_GL.h              lib/EZ_Render.h          lib/EZ_WidgetEvent.c       lib/EZ_WidgetScroll.c
  lib/EZ_ConvertLine24.h   lib/EZ_GLImage.c         lib/EZ_ResEdit.h         lib/EZ_WidgetFile.c        lib/EZ_WidgetSeparator.c
  lib/EZ_ConvertLine.c     lib/EZ_GM.c              lib/EZ_Resource.c        lib/EZ_WidgetFileSelect.c  lib/EZ_WidgetSlider.c
  lib/EZ_ConvertLine.h     lib/EZ_GM.h              lib/EZ_ResourceEdit.c    lib/EZ_WidgetFListBox.c    lib/EZ_WidgetSMeter.c
  lib/EZ_ConvertLineP.c    lib/EZ_GQuarks.h         lib/EZ_ResourceGet.c     lib/EZ_WidgetFont.c        lib/EZ_WidgetSpinButton.c
  lib/EZ_ConvertLineP.h    lib/EZ_Indicator.c       lib/EZ_Resource.h        lib/EZ_WidgetFrame.c       lib/EZ_WidgetSSheet.c
  lib/EZ_ConvertPoint00.h  lib/EZ_InitGL.c          lib/EZ_RowCol.c          lib/EZ_WidgetGBar.c        lib/EZ_WidgetTearOff.c
  lib/EZ_ConvertPoint24.h  lib/EZ_Input.c           lib/EZ_RText.c           lib/EZ_WidgetGC.c          lib/EZ_WidgetTerm.c
  lib/EZ_ConvertPoint.c    lib/EZ_Input.h           lib/EZ_Sphere.c          lib/EZ_WidgetGeom.c        lib/EZ_WidgetText.c
  lib/EZ_ConvertPoint.h    lib/EZ_ItemAttr.h        lib/EZ_Sqrt.c            lib/EZ_WidgetGrab.c        lib/EZ_WidgetThumWheel.c
  lib/EZ_ConvertPointP.c   lib/EZ_Item.c            lib/EZ_Sqrt.h            lib/EZ_Widget.h            lib/EZ_WidgetTree.c
  lib/EZ_ConvertPointP.h   lib/EZ_ItemFig.c         lib/EZ_String.c          lib/EZ_WidgetHash.c        lib/EZ_WidgetWorkArea.c
  lib/EZ_ConvertPoly00.h   lib/EZ_Item.h            lib/EZ_Test.c            lib/EZ_WidgetHistogram.c   lib/EZ_WM.c
  lib/EZ_ConvertPoly24.h   lib/EZ_ItemHash.c        lib/EZ_Text.c            lib/EZ_WidgetIFListBox.c   lib/EZ_WorkAreaGeom.c
  lib/EZ_ConvertPoly.c     lib/EZ_ItemLabel.c       lib/EZ_Text.h            lib/EZ_WidgetIListBox.c    lib/EZ_World2Screen.c
  lib/EZ_ConvertPoly.h     lib/EZ_ItemUDF.c         lib/EZ_WidgetImage.c     lib/EZ_X11Init.c
  lib/EZ_ConvertPolyP00.h  lib/EZ_ItemWidget.c      lib/EZ_WidgetInit.c      lib/EZ_X11Render.c
  lib/EZ_ConvertPolyP24.h  lib/EZ_ITermComm.c       lib/EZ_Timer.c           lib/EZ_WidgetInput.c       lib/EZ_XImageTypes.h
  lib/EZ_ConvertPolyP.c    lib/EZ_ITerm.h           lib/EZ_Timer.h           lib/EZ_WidgetITerm.c       lib/EZ_Xpm.c
  lib/EZ_ConvertPolyP.h    lib/EZ_ITermScrn.c       lib/EZ_TokenCode.h       lib/EZ_WidgetIText.c       lib/fnmatch.c
  lib/EZ_CQuarks.h         lib/EZ_Token.h           lib/EZ_WidgetITopLevel.c lib/fnmatch.h
  lib/EZ_CreateItem.c      lib/EZ_LightMaterial1.c  lib/EZ_Tree.c            lib/EZ_WidgetITree.c       lib/glob.c
  lib/EZ_CreateWidget.c    lib/EZ_LightMaterial.c   lib/EZ_TreeDir.c         lib/EZ_WidgetIWorkArea.c   lib/MemDebug.h
  lib/EZ_Cursor.c          lib/EZ_LightMaterial.h   lib/EZ_Tube.c            lib/EZ_WidgetLabel.c       lib/ndir.h
  lib/EZ_Cursor.h          lib/EZ_MatrixStack.c     lib/EZ_Types.h           lib/EZ_WidgetLcd.c         lib/tilde.c
  lib/EZ_DnDAtom.h         lib/EZ_MatrixTrans.c     lib/EZ_VectFont.h        lib/EZ_WidgetLed.c         lib/tilde.h
  lib/EZ_DnD.c             lib/EZ_MatrixVect.c      lib/EZ_Widget3DCanvas.c  lib/EZ_WidgetListBox.c     lib/VERSION.h
  lib/EZ_DnDData.c         lib/EZ_MemDebug.c        lib/EZ_WidgetArrange.c   lib/EZ_WidgetLocator.c
)

IF(NOT HAVE_ALLOCA_H)
  SET(EZWGL_SRC ${EZWGL_SRC} lib/alloca.c )
ENDIF(NOT HAVE_ALLOCA_H)
  
IF(HAVE_TIFF)
  SET(EZWGL_SRC ${EZWGL_SRC} lib/EZ_Tiff.c )
ENDIF(HAVE_TIFF)

IF(HAVE_PNG)
  SET(EZWGL_SRC ${EZWGL_SRC} lib/EZ_Png.c )
ENDIF(HAVE_PNG)

IF(HAVE_JPEG)
  SET(EZWGL_SRC ${EZWGL_SRC} lib/EZ_Jpeg.c )
ENDIF(HAVE_JPEG)


ADD_LIBRARY(ezwgl ${EZWGL_SRC})
