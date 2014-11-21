//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2002 by Denton Woods
// Last modified: 02/19/2002 <--Y2K Compliant! =]
//
// Filename: src-IL/src/il_header.c
//
// Description: Generates a C-style header file for the current image.
//
//-----------------------------------------------------------------------------

#ifndef IL_NO_CHEAD

#include "il_internal.h"

// Just a guess...let's see what's purty!
#define MAX_LINE_WIDTH 14

//! Generates a C-style header file for the current image.
ILboolean ilSaveCHeader(const ILstring FileName, const char *InternalName)
{
	FILE	*HeadFile;
	ILuint	i = 0, j;
	ILimage	*TempImage;
	char	*Name;

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	Name = iGetString(IL_CHEAD_HEADER_STRING);
	if (Name == NULL)
		Name = (char*)InternalName;

	if (FileName == NULL || Name == NULL ||
#ifndef _UNICODE
		strlen(FileName) < 1 ||	strlen(Name) < 1) {
#else
		wcslen(FileName) < 1 || wcslen(FileName) < 1) {
#endif//_UNICODE
		ilSetError(IL_INVALID_VALUE);
                ifree(Name);
		return IL_FALSE;
	}

	if (!iCheckExtension(FileName, IL_TEXT("h"))) {
		ilSetError(IL_INVALID_EXTENSION);
                ifree(Name);
		return IL_FALSE;
	}

	if (ilGetBoolean(IL_FILE_MODE) == IL_FALSE) {
		if (iFileExists(FileName)) {
			ilSetError(IL_FILE_ALREADY_EXISTS);
			ifree(Name);
                        return IL_FALSE;
		}
	}

	if (iCurImage->Bpc > 1) {
		TempImage = iConvertImage(iCurImage, iCurImage->Format, IL_UNSIGNED_BYTE);
		if (TempImage == NULL) {
                    ifree(Name);
                    return IL_FALSE;
                }
	} else {
		TempImage = iCurImage;
	}

#ifndef _WIN32_WCE
	HeadFile = fopen(FileName, "wb");
#else
	HeadFile = _wfopen(FileName, L"wb");
#endif//_WIN32_WCE

	
	if (HeadFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		ifree(Name);
                return IL_FALSE;
	}

	fprintf(HeadFile, "//#include <il/il.h>\n");
	fprintf(HeadFile, "// C Image Header:\n\n\n");
	fprintf(HeadFile, "// IMAGE_BPP is in bytes per pixel, *not* bits\n");
        fprintf(HeadFile, "#define IMAGE_BPP %d\n",iCurImage->Bpp);
	fprintf(HeadFile, "#define IMAGE_WIDTH   %d\n", iCurImage->Width);
	fprintf(HeadFile, "#define IMAGE_HEIGHT  %d\n", iCurImage->Height);	
	fprintf(HeadFile, "#define IMAGE_DEPTH   %d\n\n\n", iCurImage->Depth);
	fprintf(HeadFile, "#define IMAGE_TYPE    0x%X\n", iCurImage->Type);
	fprintf(HeadFile, "#define IMAGE_FORMAT  0x%X\n\n\n", iCurImage->Format);
        fprintf(HeadFile, "ILubyte %s[] = {\n", Name);
        

	for (; i < TempImage->SizeOfData; i += MAX_LINE_WIDTH) {
		fprintf(HeadFile, "\t");
		for (j = 0; j < MAX_LINE_WIDTH; j++) {
			if (i + j >= TempImage->SizeOfData - 1) {
				fprintf(HeadFile, "%4d", TempImage->Data[i+j]);
				break;
			}
			else
				fprintf(HeadFile, "%4d,", TempImage->Data[i+j]);
		}
		fprintf(HeadFile, "\n");
	}
	if (TempImage != iCurImage)
		ilCloseImage(TempImage);

	fprintf(HeadFile, "};\n");


	if (iCurImage->Pal.Palette && iCurImage->Pal.PalSize && iCurImage->Pal.PalType != IL_PAL_NONE) {
		fprintf(HeadFile, "\n\n");
		fprintf(HeadFile, "#define IMAGE_PALSIZE %u\n\n", iCurImage->Pal.PalSize);
		fprintf(HeadFile, "#define IMAGE_PALTYPE 0x%X\n\n", iCurImage->Pal.PalType);
                fprintf(HeadFile, "ILubyte %sPal[] = {\n", Name);
		for (i = 0; i < iCurImage->Pal.PalSize; i += MAX_LINE_WIDTH) {
			fprintf(HeadFile, "\t");
			for (j = 0; j < MAX_LINE_WIDTH; j++) {
				if (i + j >= iCurImage->Pal.PalSize - 1) {
					fprintf(HeadFile, " %4d", iCurImage->Pal.Palette[i+j]);
					break;
				}
				else
					fprintf(HeadFile, " %4d,", iCurImage->Pal.Palette[i+j]);
			}
			fprintf(HeadFile, "\n");
		}

		fprintf(HeadFile, "};\n");
	}
        ifree(Name);
	fclose(HeadFile);
	return IL_TRUE;
}



#endif//IL_NO_CHEAD
