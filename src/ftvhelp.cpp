/******************************************************************************
 * ftvhelp.cpp,v 1.0 2000/09/06 16:09:00
 *
 * Contributed by Kenney Wong <kwong@ea.com>
 * Modified by Dimitri van Heesch (c) 2003
 *
 * Folder Tree View for offline help on browsers that do not support HTML Help.
 */

#include <stdio.h>
#include <stdlib.h>
#include <qlist.h>
#include <qdict.h>
#include <qfileinfo.h>

#include "ftvhelp.h"
#include "config.h"
#include "message.h"
#include "doxygen.h"
#include "language.h"

#define MAX_INDENT 1024

unsigned char ftv2blank_png[] = {
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
  0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x16,
  0x01, 0x00, 0x00, 0x00, 0x01, 0x96, 0xd6, 0x11, 0x47, 0x00, 0x00, 0x00,
  0x02, 0x74, 0x52, 0x4e, 0x53, 0x00, 0x01, 0x01, 0x94, 0xfd, 0xae, 0x00,
  0x00, 0x00, 0x16, 0x74, 0x45, 0x58, 0x74, 0x53, 0x6f, 0x66, 0x74, 0x77,
  0x61, 0x72, 0x65, 0x00, 0x67, 0x69, 0x66, 0x32, 0x70, 0x6e, 0x67, 0x20,
  0x32, 0x2e, 0x34, 0x2e, 0x32, 0xa3, 0x5e, 0x47, 0x0e, 0x00, 0x00, 0x00,
  0x25, 0x74, 0x45, 0x58, 0x74, 0x43, 0x6f, 0x6d, 0x6d, 0x65, 0x6e, 0x74,
  0x00, 0x55, 0x6c, 0x65, 0x61, 0x64, 0x20, 0x47, 0x49, 0x46, 0x20, 0x53,
  0x6d, 0x61, 0x72, 0x74, 0x53, 0x61, 0x76, 0x65, 0x72, 0x20, 0x56, 0x65,
  0x72, 0x20, 0x32, 0x2e, 0x30, 0x21, 0xf8, 0xd7, 0x5e, 0x53, 0x00, 0x00,
  0x00, 0x14, 0x49, 0x44, 0x41, 0x54, 0x78, 0xda, 0x63, 0x38, 0xc0, 0x80,
  0x80, 0x1f, 0x30, 0xe0, 0x7f, 0x42, 0x90, 0x00, 0x02, 0x00, 0x78, 0x3c,
  0x32, 0xcb, 0x72, 0x8f, 0x7c, 0x12, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45,
  0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

unsigned char ftv2doc_png[] = {
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
  0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x16,
  0x04, 0x03, 0x00, 0x00, 0x01, 0x5f, 0x54, 0x71, 0x2d, 0x00, 0x00, 0x00,
  0x15, 0x50, 0x4c, 0x54, 0x45, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff,
  0xff, 0xff, 0xc0, 0xc0, 0xc0, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xff, 0xb3, 0xbd, 0xfb, 0xc8, 0x00, 0x00, 0x00, 0x01, 0x74, 0x52,
  0x4e, 0x53, 0x00, 0x40, 0xe6, 0xd8, 0x66, 0x00, 0x00, 0x00, 0x16, 0x74,
  0x45, 0x58, 0x74, 0x53, 0x6f, 0x66, 0x74, 0x77, 0x61, 0x72, 0x65, 0x00,
  0x67, 0x69, 0x66, 0x32, 0x70, 0x6e, 0x67, 0x20, 0x32, 0x2e, 0x34, 0x2e,
  0x32, 0xa3, 0x5e, 0x47, 0x0e, 0x00, 0x00, 0x00, 0x76, 0x49, 0x44, 0x41,
  0x54, 0x78, 0xda, 0x63, 0x60, 0x60, 0x60, 0x60, 0x63, 0x60, 0x60, 0x64,
  0x00, 0x01, 0x27, 0x30, 0x62, 0x71, 0x01, 0xe2, 0x24, 0x06, 0x38, 0x60,
  0x71, 0x00, 0xca, 0x27, 0x33, 0x30, 0x30, 0x01, 0x31, 0xa3, 0x32, 0x03,
  0x0a, 0x70, 0x54, 0x32, 0x01, 0x53, 0x69, 0x60, 0x71, 0x27, 0x08, 0x15,
  0x1a, 0x1a, 0xca, 0x80, 0x01, 0x5c, 0x5c, 0xc0, 0x94, 0xa2, 0x90, 0x02,
  0x88, 0x52, 0x4b, 0x52, 0x05, 0x53, 0x69, 0xc8, 0x94, 0x18, 0x84, 0x52,
  0x52, 0x12, 0xc5, 0x69, 0x0a, 0x36, 0xe0, 0xa4, 0xa4, 0xa4, 0x68, 0x82,
  0xc4, 0x51, 0x82, 0x6b, 0x04, 0x71, 0x14, 0x4d, 0x61, 0x4e, 0x55, 0x04,
  0xf2, 0x60, 0x1c, 0x27, 0x45, 0x24, 0x19, 0x90, 0x01, 0x70, 0x19, 0x67,
  0x63, 0x20, 0x30, 0xc5, 0x6f, 0x23, 0x00, 0xf5, 0xd0, 0x11, 0xe0, 0x55,
  0x83, 0x47, 0xbd, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae,
  0x42, 0x60, 0x82
};

unsigned char ftv2folderclosed_png[] = {
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
  0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x16,
  0x04, 0x03, 0x00, 0x00, 0x01, 0x5f, 0x54, 0x71, 0x2d, 0x00, 0x00, 0x00,
  0x12, 0x50, 0x4c, 0x54, 0x45, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0,
  0xc0, 0xc0, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x42,
  0xee, 0x40, 0xe1, 0x00, 0x00, 0x00, 0x01, 0x74, 0x52, 0x4e, 0x53, 0x00,
  0x40, 0xe6, 0xd8, 0x66, 0x00, 0x00, 0x00, 0x16, 0x74, 0x45, 0x58, 0x74,
  0x53, 0x6f, 0x66, 0x74, 0x77, 0x61, 0x72, 0x65, 0x00, 0x67, 0x69, 0x66,
  0x32, 0x70, 0x6e, 0x67, 0x20, 0x32, 0x2e, 0x34, 0x2e, 0x32, 0xa3, 0x5e,
  0x47, 0x0e, 0x00, 0x00, 0x00, 0x7d, 0x49, 0x44, 0x41, 0x54, 0x78, 0xda,
  0x8d, 0x8e, 0xc1, 0x0a, 0x84, 0x30, 0x10, 0x43, 0x23, 0x8c, 0x77, 0x0f,
  0xfb, 0x03, 0x42, 0xbd, 0x2b, 0xf8, 0x01, 0x65, 0x6d, 0xef, 0xa2, 0xcd,
  0xff, 0xff, 0x8a, 0x33, 0xad, 0xee, 0x5a, 0xf6, 0xb0, 0x06, 0xda, 0xf0,
  0x86, 0xa6, 0x13, 0x00, 0x88, 0x40, 0x03, 0x53, 0x02, 0x82, 0x9a, 0xd7,
  0x51, 0x42, 0x25, 0xae, 0x7a, 0x76, 0xa0, 0xed, 0xea, 0x79, 0x79, 0x17,
  0x49, 0x73, 0xe1, 0xf2, 0x32, 0x14, 0x8f, 0x5f, 0x49, 0xb9, 0xed, 0x23,
  0x60, 0x20, 0xcd, 0x36, 0x66, 0xe4, 0x40, 0x4b, 0xb4, 0xdb, 0xdb, 0xe5,
  0x16, 0xee, 0x16, 0x78, 0x20, 0xf9, 0x96, 0x15, 0x6d, 0xc2, 0x8b, 0xa2,
  0x6e, 0xf9, 0x50, 0x64, 0xa6, 0x70, 0xb6, 0x50, 0x0a, 0xd3, 0x78, 0x86,
  0x98, 0xa6, 0xde, 0x5d, 0x9d, 0x25, 0xcd, 0xfe, 0xdf, 0xc6, 0x03, 0xa0,
  0x13, 0x15, 0x98, 0x60, 0xbd, 0x81, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x49,
  0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

unsigned char ftv2folderopen_png[] = {
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
  0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x16,
  0x04, 0x03, 0x00, 0x00, 0x01, 0x5f, 0x54, 0x71, 0x2d, 0x00, 0x00, 0x00,
  0x15, 0x50, 0x4c, 0x54, 0x45, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff,
  0xff, 0xff, 0xc0, 0xc0, 0xc0, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x80,
  0x00, 0x80, 0x92, 0x32, 0x8c, 0xe5, 0x00, 0x00, 0x00, 0x01, 0x74, 0x52,
  0x4e, 0x53, 0x00, 0x40, 0xe6, 0xd8, 0x66, 0x00, 0x00, 0x00, 0x16, 0x74,
  0x45, 0x58, 0x74, 0x53, 0x6f, 0x66, 0x74, 0x77, 0x61, 0x72, 0x65, 0x00,
  0x67, 0x69, 0x66, 0x32, 0x70, 0x6e, 0x67, 0x20, 0x32, 0x2e, 0x34, 0x2e,
  0x32, 0xa3, 0x5e, 0x47, 0x0e, 0x00, 0x00, 0x00, 0x7c, 0x49, 0x44, 0x41,
  0x54, 0x78, 0xda, 0x85, 0x8d, 0x41, 0x0e, 0x82, 0x30, 0x10, 0x45, 0x5f,
  0x08, 0xf5, 0x1e, 0x2d, 0x53, 0xd6, 0xc6, 0x18, 0xd7, 0x8d, 0x18, 0xd7,
  0x06, 0xd2, 0x1e, 0xa0, 0xb1, 0xde, 0xff, 0x08, 0x3a, 0x20, 0x21, 0x10,
  0xa2, 0x6f, 0xf3, 0xfe, 0xff, 0x8b, 0x19, 0x80, 0x0a, 0x6a, 0x14, 0x81,
  0x3c, 0x06, 0x2a, 0x61, 0xc5, 0xd3, 0x43, 0x69, 0xc1, 0xb4, 0xab, 0xf9,
  0x70, 0xac, 0x83, 0xca, 0xbb, 0xfb, 0xa4, 0x8b, 0xae, 0x26, 0x46, 0xb6,
  0x4c, 0x0f, 0xe8, 0xc3, 0x18, 0xfa, 0xb3, 0x7d, 0xa8, 0x1a, 0xeb, 0x17,
  0xa5, 0x46, 0x54, 0x46, 0x4e, 0x2a, 0xe2, 0xce, 0x95, 0x3d, 0xba, 0xb0,
  0x64, 0x93, 0x85, 0x9b, 0x0b, 0x73, 0x71, 0x83, 0x75, 0xd7, 0xf2, 0xa7,
  0x90, 0xf2, 0x20, 0x9d, 0xfb, 0x16, 0xd2, 0xeb, 0x43, 0xf9, 0xfd, 0xf2,
  0x0d, 0xa4, 0x29, 0x14, 0xcb, 0xda, 0x47, 0xac, 0x44, 0x00, 0x00, 0x00,
  0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

unsigned char ftv2lastnode_png[] = {
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
  0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x16,
  0x04, 0x03, 0x00, 0x00, 0x01, 0x4c, 0x83, 0x31, 0xd9, 0x00, 0x00, 0x00,
  0x30, 0x50, 0x4c, 0x54, 0x45, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x80, 0x80, 0x80, 0x18, 0xd3, 0xa0, 0x90, 0x00, 0x00, 0x00,
  0x01, 0x74, 0x52, 0x4e, 0x53, 0x00, 0x40, 0xe6, 0xd8, 0x66, 0x00, 0x00,
  0x00, 0x16, 0x74, 0x45, 0x58, 0x74, 0x53, 0x6f, 0x66, 0x74, 0x77, 0x61,
  0x72, 0x65, 0x00, 0x67, 0x69, 0x66, 0x32, 0x70, 0x6e, 0x67, 0x20, 0x32,
  0x2e, 0x34, 0x2e, 0x32, 0xa3, 0x5e, 0x47, 0x0e, 0x00, 0x00, 0x00, 0x26,
  0x74, 0x45, 0x58, 0x74, 0x43, 0x6f, 0x6d, 0x6d, 0x65, 0x6e, 0x74, 0x00,
  0x55, 0x6c, 0x65, 0x61, 0x64, 0x20, 0x47, 0x49, 0x46, 0x20, 0x53, 0x6d,
  0x61, 0x72, 0x74, 0x53, 0x61, 0x76, 0x65, 0x72, 0x20, 0x56, 0x65, 0x72,
  0x20, 0x32, 0x2e, 0x30, 0x69, 0x01, 0x6f, 0x3f, 0xcd, 0x06, 0x00, 0x00,
  0x00, 0x13, 0x49, 0x44, 0x41, 0x54, 0x78, 0xda, 0x63, 0x60, 0x18, 0x30,
  0xc0, 0x4f, 0x0c, 0x03, 0x04, 0xc8, 0x33, 0x1e, 0x00, 0x24, 0xa7, 0x00,
  0x88, 0x10, 0xca, 0x33, 0x3d, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e,
  0x44, 0xae, 0x42, 0x60, 0x82
};

unsigned char ftv2link_png[] = {
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
  0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x16,
  0x04, 0x03, 0x00, 0x00, 0x01, 0x5f, 0x54, 0x71, 0x2d, 0x00, 0x00, 0x00,
  0x30, 0x50, 0x4c, 0x54, 0x45, 0xff, 0xff, 0xff, 0x00, 0x80, 0x00, 0xff,
  0xff, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0xff, 0x00, 0x80, 0x80, 0x80,
  0x80, 0x80, 0xc0, 0xc0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x7e, 0x9b, 0x08, 0x00, 0x00, 0x00,
  0x01, 0x74, 0x52, 0x4e, 0x53, 0x00, 0x40, 0xe6, 0xd8, 0x66, 0x00, 0x00,
  0x00, 0x16, 0x74, 0x45, 0x58, 0x74, 0x53, 0x6f, 0x66, 0x74, 0x77, 0x61,
  0x72, 0x65, 0x00, 0x67, 0x69, 0x66, 0x32, 0x70, 0x6e, 0x67, 0x20, 0x32,
  0x2e, 0x34, 0x2e, 0x32, 0xa3, 0x5e, 0x47, 0x0e, 0x00, 0x00, 0x00, 0x26,
  0x74, 0x45, 0x58, 0x74, 0x43, 0x6f, 0x6d, 0x6d, 0x65, 0x6e, 0x74, 0x00,
  0x55, 0x6c, 0x65, 0x61, 0x64, 0x20, 0x47, 0x49, 0x46, 0x20, 0x53, 0x6d,
  0x61, 0x72, 0x74, 0x53, 0x61, 0x76, 0x65, 0x72, 0x20, 0x56, 0x65, 0x72,
  0x20, 0x32, 0x2e, 0x30, 0x19, 0x02, 0xd9, 0x09, 0xe5, 0x4a, 0x00, 0x00,
  0x00, 0x90, 0x49, 0x44, 0x41, 0x54, 0x78, 0xda, 0x7d, 0x8c, 0xb1, 0x0a,
  0xc2, 0x30, 0x14, 0x45, 0x6f, 0x33, 0xb4, 0x63, 0x7e, 0xa1, 0x53, 0x57,
  0x8b, 0x7e, 0x80, 0x6e, 0xae, 0x16, 0x5a, 0xb2, 0xbe, 0x3a, 0x64, 0x35,
  0xb8, 0x64, 0xec, 0x2f, 0x07, 0x05, 0x1d, 0x9f, 0x79, 0x8d, 0x21, 0x50,
  0xd0, 0x03, 0xc9, 0xcd, 0xe1, 0xbe, 0x17, 0x00, 0xa8, 0x00, 0x82, 0xe0,
  0x81, 0x66, 0x7d, 0xc0, 0x9e, 0x51, 0x70, 0x40, 0x1d, 0xcf, 0xad, 0x07,
  0x18, 0x1b, 0x74, 0x90, 0xdb, 0x1c, 0x62, 0x0f, 0xe2, 0x20, 0xc1, 0xcc,
  0x61, 0x3b, 0x06, 0x6a, 0xbb, 0x14, 0x3b, 0xe9, 0x68, 0xba, 0xac, 0x51,
  0x9d, 0x24, 0xec, 0xdc, 0x4b, 0x68, 0xed, 0xd2, 0x5e, 0x1b, 0x7e, 0xfd,
  0x92, 0xa8, 0x6d, 0x84, 0xb2, 0x28, 0x28, 0x38, 0xca, 0xd2, 0x4c, 0x46,
  0x2d, 0x59, 0xcc, 0x38, 0x5c, 0x8b, 0xcc, 0xe3, 0xfe, 0x2b, 0x71, 0xfa,
  0x79, 0xbc, 0x67, 0x79, 0x85, 0x07, 0x54, 0x69, 0xde, 0xde, 0x2f, 0xf8,
  0xcf, 0x07, 0x71, 0x95, 0x2b, 0xa1, 0x10, 0x78, 0xd0, 0xff, 0x00, 0x00,
  0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

unsigned char ftv2mlastnode_png[] = {
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
  0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x16,
  0x02, 0x03, 0x00, 0x00, 0x01, 0xc3, 0xc3, 0xc4, 0x79, 0x00, 0x00, 0x00,
  0x09, 0x50, 0x4c, 0x54, 0x45, 0xff, 0xff, 0xff, 0x80, 0x80, 0x80, 0x00,
  0x00, 0x00, 0x3c, 0x5e, 0xbb, 0x2c, 0x00, 0x00, 0x00, 0x01, 0x74, 0x52,
  0x4e, 0x53, 0x00, 0x40, 0xe6, 0xd8, 0x66, 0x00, 0x00, 0x00, 0x16, 0x74,
  0x45, 0x58, 0x74, 0x53, 0x6f, 0x66, 0x74, 0x77, 0x61, 0x72, 0x65, 0x00,
  0x67, 0x69, 0x66, 0x32, 0x70, 0x6e, 0x67, 0x20, 0x32, 0x2e, 0x34, 0x2e,
  0x32, 0xa3, 0x5e, 0x47, 0x0e, 0x00, 0x00, 0x00, 0x23, 0x49, 0x44, 0x41,
  0x54, 0x78, 0xda, 0x63, 0x60, 0x20, 0x0b, 0x08, 0x08, 0x20, 0x10, 0x0a,
  0x60, 0x84, 0x11, 0x8c, 0xa1, 0xa1, 0x20, 0x06, 0x90, 0xc9, 0xa8, 0xb5,
  0x50, 0x10, 0xca, 0x02, 0x89, 0x61, 0x01, 0x00, 0x6d, 0x17, 0x02, 0xba,
  0xc0, 0xdc, 0x69, 0xc8, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44,
  0xae, 0x42, 0x60, 0x82
};

unsigned char ftv2mnode_png[] = {
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
  0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x16,
  0x04, 0x03, 0x00, 0x00, 0x01, 0x4c, 0x83, 0x31, 0xd9, 0x00, 0x00, 0x00,
  0x24, 0x50, 0x4c, 0x54, 0x45, 0xc0, 0xc0, 0xc0, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x80, 0x80, 0x80, 0x53, 0xbe, 0x1e, 0x99, 0x00, 0x00, 0x00,
  0x01, 0x74, 0x52, 0x4e, 0x53, 0x00, 0x40, 0xe6, 0xd8, 0x66, 0x00, 0x00,
  0x00, 0x16, 0x74, 0x45, 0x58, 0x74, 0x53, 0x6f, 0x66, 0x74, 0x77, 0x61,
  0x72, 0x65, 0x00, 0x67, 0x69, 0x66, 0x32, 0x70, 0x6e, 0x67, 0x20, 0x32,
  0x2e, 0x34, 0x2e, 0x32, 0xa3, 0x5e, 0x47, 0x0e, 0x00, 0x00, 0x00, 0x2a,
  0x49, 0x44, 0x41, 0x54, 0x78, 0xda, 0x63, 0x60, 0xa0, 0x2e, 0xe0, 0x06,
  0x42, 0x74, 0x02, 0xa7, 0x52, 0x54, 0x06, 0xf7, 0x6e, 0x20, 0x80, 0xf1,
  0xc1, 0x62, 0xdc, 0x4c, 0x4a, 0x4a, 0xdc, 0xdc, 0xdc, 0xc8, 0x22, 0x30,
  0x35, 0x98, 0xda, 0xd1, 0x19, 0x00, 0xb7, 0x79, 0x07, 0x27, 0xaa, 0xf7,
  0x96, 0x03, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42,
  0x60, 0x82
};

unsigned char ftv2node_png[] = {
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
  0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x16,
  0x04, 0x03, 0x00, 0x00, 0x01, 0x4c, 0x83, 0x31, 0xd9, 0x00, 0x00, 0x00,
  0x30, 0x50, 0x4c, 0x54, 0x45, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x80, 0x80, 0x80, 0x18, 0xd3, 0xa0, 0x90, 0x00, 0x00, 0x00,
  0x01, 0x74, 0x52, 0x4e, 0x53, 0x00, 0x40, 0xe6, 0xd8, 0x66, 0x00, 0x00,
  0x00, 0x16, 0x74, 0x45, 0x58, 0x74, 0x53, 0x6f, 0x66, 0x74, 0x77, 0x61,
  0x72, 0x65, 0x00, 0x67, 0x69, 0x66, 0x32, 0x70, 0x6e, 0x67, 0x20, 0x32,
  0x2e, 0x34, 0x2e, 0x32, 0xa3, 0x5e, 0x47, 0x0e, 0x00, 0x00, 0x00, 0x26,
  0x74, 0x45, 0x58, 0x74, 0x43, 0x6f, 0x6d, 0x6d, 0x65, 0x6e, 0x74, 0x00,
  0x55, 0x6c, 0x65, 0x61, 0x64, 0x20, 0x47, 0x49, 0x46, 0x20, 0x53, 0x6d,
  0x61, 0x72, 0x74, 0x53, 0x61, 0x76, 0x65, 0x72, 0x20, 0x56, 0x65, 0x72,
  0x20, 0x32, 0x2e, 0x30, 0x69, 0x01, 0x6f, 0x3f, 0xcd, 0x06, 0x00, 0x00,
  0x00, 0x15, 0x49, 0x44, 0x41, 0x54, 0x78, 0xda, 0x63, 0x60, 0x18, 0x30,
  0xc0, 0x4f, 0x0c, 0x03, 0x04, 0x88, 0x56, 0x8c, 0xc2, 0x00, 0x00, 0x2e,
  0x52, 0x00, 0xe2, 0xfa, 0x45, 0x3a, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x49,
  0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

unsigned char ftv2plastnode_png[] = {
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
  0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x16,
  0x02, 0x03, 0x00, 0x00, 0x01, 0xc3, 0xc3, 0xc4, 0x79, 0x00, 0x00, 0x00,
  0x09, 0x50, 0x4c, 0x54, 0x45, 0xff, 0xff, 0xff, 0x80, 0x80, 0x80, 0x00,
  0x00, 0x00, 0x3c, 0x5e, 0xbb, 0x2c, 0x00, 0x00, 0x00, 0x01, 0x74, 0x52,
  0x4e, 0x53, 0x00, 0x40, 0xe6, 0xd8, 0x66, 0x00, 0x00, 0x00, 0x16, 0x74,
  0x45, 0x58, 0x74, 0x53, 0x6f, 0x66, 0x74, 0x77, 0x61, 0x72, 0x65, 0x00,
  0x67, 0x69, 0x66, 0x32, 0x70, 0x6e, 0x67, 0x20, 0x32, 0x2e, 0x34, 0x2e,
  0x32, 0xa3, 0x5e, 0x47, 0x0e, 0x00, 0x00, 0x00, 0x28, 0x49, 0x44, 0x41,
  0x54, 0x78, 0xda, 0x63, 0x60, 0x20, 0x0b, 0x08, 0x08, 0x30, 0x08, 0x81,
  0x11, 0x90, 0x81, 0x02, 0x18, 0x61, 0x04, 0x63, 0x68, 0x28, 0x90, 0x60,
  0x02, 0x32, 0x19, 0xb5, 0x16, 0x0a, 0x42, 0x59, 0x20, 0x31, 0x2c, 0x00,
  0x00, 0x6e, 0xc1, 0x02, 0xc2, 0xe5, 0xed, 0x75, 0xa7, 0x00, 0x00, 0x00,
  0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

unsigned char ftv2pnode_png[] = {
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
  0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x16,
  0x04, 0x03, 0x00, 0x00, 0x01, 0x4c, 0x83, 0x31, 0xd9, 0x00, 0x00, 0x00,
  0x24, 0x50, 0x4c, 0x54, 0x45, 0xc0, 0xc0, 0xc0, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x80, 0x80, 0x80, 0x53, 0xbe, 0x1e, 0x99, 0x00, 0x00, 0x00,
  0x01, 0x74, 0x52, 0x4e, 0x53, 0x00, 0x40, 0xe6, 0xd8, 0x66, 0x00, 0x00,
  0x00, 0x16, 0x74, 0x45, 0x58, 0x74, 0x53, 0x6f, 0x66, 0x74, 0x77, 0x61,
  0x72, 0x65, 0x00, 0x67, 0x69, 0x66, 0x32, 0x70, 0x6e, 0x67, 0x20, 0x32,
  0x2e, 0x34, 0x2e, 0x32, 0xa3, 0x5e, 0x47, 0x0e, 0x00, 0x00, 0x00, 0x30,
  0x49, 0x44, 0x41, 0x54, 0x78, 0xda, 0x63, 0x60, 0xa0, 0x2e, 0xe0, 0x06,
  0x42, 0x06, 0x6e, 0x26, 0x38, 0x01, 0xe2, 0xe2, 0x54, 0x8a, 0xca, 0xe0,
  0xde, 0x0d, 0x04, 0x10, 0x3e, 0x13, 0x44, 0x8c, 0x9b, 0x49, 0x49, 0x89,
  0x9b, 0x9b, 0x1b, 0x59, 0x04, 0xa6, 0x06, 0x53, 0x3b, 0x3a, 0x03, 0x00,
  0xba, 0x6b, 0x07, 0x2f, 0xaa, 0xcb, 0x1f, 0x6f, 0x00, 0x00, 0x00, 0x00,
  0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

unsigned char ftv2vertline_png[] = {
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
  0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x16,
  0x04, 0x03, 0x00, 0x00, 0x01, 0x4c, 0x83, 0x31, 0xd9, 0x00, 0x00, 0x00,
  0x30, 0x50, 0x4c, 0x54, 0x45, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x80, 0x80, 0x80, 0x18, 0xd3, 0xa0, 0x90, 0x00, 0x00, 0x00,
  0x01, 0x74, 0x52, 0x4e, 0x53, 0x00, 0x40, 0xe6, 0xd8, 0x66, 0x00, 0x00,
  0x00, 0x16, 0x74, 0x45, 0x58, 0x74, 0x53, 0x6f, 0x66, 0x74, 0x77, 0x61,
  0x72, 0x65, 0x00, 0x67, 0x69, 0x66, 0x32, 0x70, 0x6e, 0x67, 0x20, 0x32,
  0x2e, 0x34, 0x2e, 0x32, 0xa3, 0x5e, 0x47, 0x0e, 0x00, 0x00, 0x00, 0x26,
  0x74, 0x45, 0x58, 0x74, 0x43, 0x6f, 0x6d, 0x6d, 0x65, 0x6e, 0x74, 0x00,
  0x55, 0x6c, 0x65, 0x61, 0x64, 0x20, 0x47, 0x49, 0x46, 0x20, 0x53, 0x6d,
  0x61, 0x72, 0x74, 0x53, 0x61, 0x76, 0x65, 0x72, 0x20, 0x56, 0x65, 0x72,
  0x20, 0x32, 0x2e, 0x30, 0x69, 0x01, 0x6f, 0x3f, 0xcd, 0x06, 0x00, 0x00,
  0x00, 0x0f, 0x49, 0x44, 0x41, 0x54, 0x78, 0xda, 0x63, 0x60, 0x18, 0x30,
  0xc0, 0x4f, 0x5b, 0x06, 0x00, 0x21, 0x14, 0x00, 0xa6, 0xe5, 0x3c, 0xe8,
  0x3a, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60,
  0x82
};


FTVImageInfo image_info[] =
{
  { "&nbsp;", "ftv2blank.png",ftv2blank_png,174,16,22 },
  { "*",  "ftv2doc.png",ftv2doc_png,255,24,22 },
  { "+",  "ftv2folderclosed.png",ftv2folderclosed_png,259,24,22 },
  { "-",  "ftv2folderopen.png",ftv2folderopen_png,261,24,22 },
  { "\\", "ftv2lastnode.png",ftv2lastnode_png,233,16,22 },
  { "-",  "ftv2link.png",ftv2link_png,358,24,22 },
  { "\\", "ftv2mlastnode.png",ftv2mlastnode_png,160,16,22 },
  { "o",  "ftv2mnode.png",ftv2mnode_png,194,16,22 },
  { "o",  "ftv2node.png",ftv2node_png,235,16,22 },
  { "\\", "ftv2plastnode.png",ftv2plastnode_png,165,16,22 },
  { "o",  "ftv2pnode.png",ftv2pnode_png,200,16,22 },
  { "|",  "ftv2vertline.png",ftv2vertline_png,229,16,22 },
  { 0,0,0,0,0,0 }
};

struct FTVNode
{
  FTVNode(bool dir,const char *r,const char *f,const char *a,const char *n)
    : isLast(TRUE), isDir(dir),ref(r),file(f),anchor(a),name(n), 
      parent(0) { children.setAutoDelete(TRUE); }
  bool isLast;
  bool isDir;
  QCString ref;
  QCString file;
  QCString anchor;
  QCString name;
  QList<FTVNode> children;
  FTVNode *parent;
};


//----------------------------------------------------------------------------

FTVHelp *FTVHelp::m_theInstance = 0;

/*! Construm_cts an ftv help object. 
 *  The object has to be \link initialize() initialized\endlink before it can 
 *  be used.
 */
FTVHelp::FTVHelp() 
{
  /* initial depth */
  //m_dc = 0;
  //m_cf = 0;
  m_indentNodes = new QList<FTVNode>[MAX_INDENT];
  m_indentNodes[0].setAutoDelete(TRUE);
  m_indent=0;
}

FTVHelp::~FTVHelp()
{
  delete[] m_indentNodes;
}

/*! return a reference to the one and only instance of this class. 
 */
FTVHelp *FTVHelp::getInstance()
{
  if (m_theInstance==0) m_theInstance = new FTVHelp;
  return m_theInstance;
}

/*! This will create a folder tree view table of contents file (tree.js).
 *  \sa finalize()
 */
void FTVHelp::initialize()
{
}

/*! Finalizes the FTV help. This will finish and close the
 *  contents file (index.js).
 *  \sa initialize()
 */
void FTVHelp::finalize()
{
  generateTreeView();
}

/*! Increase the level of the contents hierarchy. 
 *  This will start a new sublist in contents file.
 *  \sa decContentsDepth()
 */
int FTVHelp::incContentsDepth()
{
  //int i; for (i=0;i<m_dc+1;i++) m_cts << "  ";
  m_indent++;
  ASSERT(m_indent<MAX_INDENT);
  return m_indent;
}

/*! Decrease the level of the contents hierarchy.
 *  This will end the current sublist.
 *  \sa incContentsDepth()
 */
int FTVHelp::decContentsDepth()
{
  //int i; for (i=0;i<m_dc;i++) m_cts << "  ";

  ASSERT(m_indent>0);
  if (m_indent>0)
  {
    m_indent--;
    QList<FTVNode> *nl = &m_indentNodes[m_indent];
    FTVNode *parent = nl->getLast();
    QList<FTVNode> *children = &m_indentNodes[m_indent+1];
    while (!children->isEmpty())
    {
      parent->children.append(children->take(0));
    }
  }
  return m_indent;
}

/*! Add a list item to the contents file.
 *  \param isDir TRUE if the item is a directory, FALSE if it is a text
 *  \param ref  the URL of to the item.
 *  \param file the file containing the definition of the item
 *  \param anchor the anchor within the file.
 *  \param name the name of the item.
 */
void FTVHelp::addContentsItem(bool isDir,
                              const char *ref,
                              const char *file,
                              const char *anchor, 
                              const char *name
                             )
{
  QList<FTVNode> *nl = &m_indentNodes[m_indent];
  FTVNode *newNode = new FTVNode(isDir,ref,file,anchor,name);
  if (!nl->isEmpty())
  {
    nl->getLast()->isLast=FALSE;
  }
  nl->append(newNode);
  if (m_indent>0)
  {
    QList<FTVNode> *pnl = &m_indentNodes[m_indent-1];
    newNode->parent = pnl->getLast();
  }
  
}

static int folderId=1;

void FTVHelp::generateIndent(QTextStream &t, FTVNode *n,int level)
{
  if (n->parent)
  {
    generateIndent(t,n->parent,level+1);
  }
  // from the root up to node n do...
  if (level==0) // item before a dir or document
  {
    if (n->isLast)
    {
      if (n->isDir)
      {
        t << "<img " << FTV_IMGATTRIBS(plastnode) << "onclick=\"toggleFolder('folder" << folderId << "', this)\"/>";
      }
      else
      {
        t << "<img " << FTV_IMGATTRIBS(lastnode) << "/>";
      }
    }
    else
    {
      if (n->isDir)
      {
        t << "<img " << FTV_IMGATTRIBS(pnode) << "onclick=\"toggleFolder('folder" << folderId << "', this)\"/>";
      }
      else
      {
        t << "<img " << FTV_IMGATTRIBS(node) << "/>";
      }
    }
  }
  else // item at another level
  {
    if (n->isLast)
    {
      t << "<img " << FTV_IMGATTRIBS(blank) << "/>";
    }
    else
    {
      t << "<img " << FTV_IMGATTRIBS(vertline) << "/>";
    }
  }
}

void FTVHelp::generateLink(QTextStream &t,FTVNode *n)
{
  QCString *dest;
  //printf("FTVHelp::generateLink(ref=%s,file=%s,anchor=%s\n",
  //    n->ref.data(),n->file.data(),n->anchor.data());
  if (n->file.isEmpty()) // no link
  {
    t << "<b>" << n->name << "</b>";
  }
  else // link into other frame
  {
    if (!n->ref.isEmpty()) // link to entity imported via tag file
    {
      t << "<a class=\"elRef\" ";
      t << "doxygen=\"" << n->ref << ":";
      if ((dest=Doxygen::tagDestinationDict[n->ref])) t << *dest << "/";
      t << "\" ";
    }
    else // local link
    {
      t << "<a class=\"el\" ";
    }
    t << "href=\"";
    if (!n->ref.isEmpty())
    {
      if ((dest=Doxygen::tagDestinationDict[n->ref])) t << *dest << "/";
    }
    t << n->file << Doxygen::htmlFileExtension;
    if (!n->anchor.isEmpty()) t << "#" << n->anchor;
    t << "\" target=\"basefrm\">";
    t << n->name;
    t << "</a>";
    if (!n->ref.isEmpty())
    {
      t << "&nbsp;[external]";
    }
  }
}

void FTVHelp::generateTree(QTextStream &t, const QList<FTVNode> &nl,int level)
{
  QCString spaces;
  spaces.fill(' ',level*2+8);
  QListIterator<FTVNode> nli(nl);
  FTVNode *n;
  for (nli.toFirst();(n=nli.current());++nli)
  {
    t << spaces << "<p>";
    generateIndent(t,n,0);
    if (n->isDir)
    {
      t << "<img " << FTV_IMGATTRIBS(folderclosed) << "onclick=\"toggleFolder('folder" << folderId << "', this)\"/>";
      generateLink(t,n);
      t << "</p>\n";
      t << spaces << "<div id=\"folder" << folderId << "\">\n";
      folderId++;
      generateTree(t,n->children,level+1);
      t << spaces << "</div>\n";
    }
    else
    {
      t << "<img " << FTV_IMGATTRIBS(doc) << "/>";
      generateLink(t,n);
      t << "</p>\n";
    }
  }
}

void FTVHelp::generateTreeViewImages()
{
  static bool done=FALSE;
  if (done) return;
  done=TRUE;

  // Generate tree view images
  FTVImageInfo *p = image_info;
  while (p->name)
  {
    QCString fileName=Config_getString("HTML_OUTPUT")+"/"+p->name;
    QFile f(fileName);
    if (f.open(IO_WriteOnly)) 
    {
      f.writeBlock((char *)p->data,p->len);
    }
    else
    {
      fprintf(stderr,"Warning: Cannot open file %s for writing\n",fileName.data());
    }
    f.close();
    p++;
  }
} 

void FTVHelp::generateTreeView()
{
  // Generate alternative index.html as a frame
  QCString fileName=Config_getString("HTML_OUTPUT")+"/index"+Doxygen::htmlFileExtension;
  QFile f;
  f.setName(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n",fileName.data());
    return;
  }
  else
  {
    QTextStream t(&f);
#if QT_VERSION >= 200
    t.setEncoding(QTextStream::Latin1);
#endif
    t << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n";
    t << "<html><head>";
    t << "<meta http-equiv=\"Content-Type\" content=\"text/html;charset="
      << theTranslator->idLanguageCharset() << "\">\n";
    t << "<title>"; 
    if (Config_getString("PROJECT_NAME").isEmpty())
    {
      t << "Doxygen Documentation";
    }
    else
    {
      t << Config_getString("PROJECT_NAME");
    }
    t << "</title></head>" << endl;
    t << "<frameset cols=\"" << Config_getInt("TREEVIEW_WIDTH") << ",*\">" << endl;
    t << "  <frame src=\"tree" << Doxygen::htmlFileExtension << "\" name=\"treefrm\">" << endl;
    t << "  <frame src=\"main" << Doxygen::htmlFileExtension << "\" name=\"basefrm\">" << endl;
    t << "</frameset>" << endl;
    t << "</html>" << endl;
    f.close();
  }

  // Generate tree view frame
  fileName=Config_getString("HTML_OUTPUT")+"/tree"+Doxygen::htmlFileExtension;
  f.setName(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n",fileName.data());
    return;
  }
  else
  {
    QTextStream t(&f);
#if QT_VERSION >= 200
    t.setEncoding(QTextStream::Latin1);
#endif
    t << "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n";
    t << "  <head>\n";
    t << "    <meta http-equiv=\"Content-Type\" content=\"text/xhtml;charset="
      << theTranslator->idLanguageCharset() << "\" />\n";
    t << "    <meta http-equiv=\"Content-Style-Type\" content=\"text/css\" />\n";
    t << "    <meta http-equiv=\"Content-Language\" content=\"en\" />\n";
    t << "    <link rel=\"stylesheet\" href=\"";
    QCString cssname=Config_getString("HTML_STYLESHEET");
    if (cssname.isEmpty())
    {
      t << "doxygen.css";
    }
    else
    {
      QFileInfo cssfi(cssname);
      if (!cssfi.exists())
      {
        err("Error: user specified HTML style sheet file does not exist!\n");
      }
      t << cssfi.fileName();
    }
    t << "\">" << endl;
    t << "    <title>TreeView</title>\n";
    t << "    <style type=\"text/css\">\n";
    t << "    <!--\n";
    t << "    .directory { font-size: 10pt; font-weight: bold; }\n";
    t << "    .directory h3 { margin: 0px; margin-top: 1em; font-size: 11pt; }\n";
    t << "    .directory p { margin: 0px; white-space: nowrap; }\n";
    t << "    .directory div { display: none; margin: 0px; }\n";
    t << "    .directory img { vertical-align: middle; }\n";
    t << "    -->\n";
    t << "    </style>\n";
    t << "    <script type=\"text/javascript\">\n";
    t << "    <!-- // Hide script from old browsers\n";
    t << "    \n";
    t << "    function findChildNode(node, name) \n";
    t << "    {\n";
    t << "      var temp;\n";
    t << "      if (node == null) \n";
    t << "      {\n";
    t << "        return null;\n";
    t << "      } \n";
    t << "      node = node.firstChild;\n";
    t << "      while (node != null) \n";
    t << "      {\n";
    t << "        if (node.nodeName == name) \n";
    t << "        {\n";
    t << "          return node;\n";
    t << "        }\n";
    t << "        temp = findChildNode(node, name);\n";
    t << "        if (temp != null) \n";
    t << "        {\n";
    t << "          return temp;\n";
    t << "        }\n";
    t << "        node = node.nextSibling;\n";
    t << "      }\n";
    t << "      return null;\n";
    t << "    }\n";
    t << "\n";
    t << "    function toggleFolder(id, imageNode) \n";
    t << "    {\n";
    t << "      var folder = document.getElementById(id);\n";
    t << "      var l = 0;\n";
    t << "      var vl = \"" FTV_ICON_FILE(vertline) "\";\n";
    t << "      if (imageNode != null && imageNode.nodeName != \"IMG\") \n";
    t << "      {\n";
    t << "        imageNode = findChildNode(imageNode, \"IMG\");\n";
    t << "        if (imageNode!=null) l = imageNode.src.length;\n";
    t << "      }\n";
    t << "      if (folder == null) \n";
    t << "      {\n";
    t << "      } \n";
    t << "      else if (folder.style.display == \"block\") \n";
    t << "      {\n";
    t << "        while (imageNode != null && \n";
    t << "               imageNode.src.substring(l-vl.length,l) == vl)\n";
    t << "        {\n";
    t << "          imageNode = imageNode.nextSibling;\n";
    t << "          l = imageNode.src.length;\n";
    t << "        }\n";
    t << "        if (imageNode != null) \n";
    t << "        {\n";
    t << "          l = imageNode.src.length;\n";
    t << "          imageNode.nextSibling.src = \"" FTV_ICON_FILE(folderclosed) "\";\n";
    t << "          if (imageNode.src.substring(l-13,l) == \"" FTV_ICON_FILE(mnode) "\")\n";
    t << "          {\n";
    t << "            imageNode.src = \"" FTV_ICON_FILE(pnode) "\";\n";
    t << "          }\n";
    t << "          else if (imageNode.src.substring(l-17,l) == \"" FTV_ICON_FILE(mlastnode) "\")\n";
    t << "          {\n";
    t << "            imageNode.src = \"" FTV_ICON_FILE(plastnode) "\";\n";
    t << "          }\n";
    t << "        }\n";
    t << "        folder.style.display = \"none\";\n";
    t << "      } \n";
    t << "      else \n";
    t << "      {\n";
    t << "        while (imageNode != null && \n";
    t << "               imageNode.src.substring(l-vl.length,l) == vl)\n";
    t << "        {\n";
    t << "          imageNode = imageNode.nextSibling;\n";
    t << "          l = imageNode.src.length;\n";
    t << "        }\n";
    t << "        if (imageNode != null) \n";
    t << "        {\n";
    t << "          l = imageNode.src.length;\n";
    t << "          imageNode.nextSibling.src = \"" FTV_ICON_FILE(folderopen) "\";\n";
    t << "          if (imageNode.src.substring(l-13,l) == \"" FTV_ICON_FILE(pnode) "\")\n";
    t << "          {\n";
    t << "            imageNode.src = \"" FTV_ICON_FILE(mnode) "\";\n";
    t << "          }\n";
    t << "          else if (imageNode.src.substring(l-17,l) == \"" FTV_ICON_FILE(plastnode) "\")\n";
    t << "          {\n";
    t << "            imageNode.src = \"" FTV_ICON_FILE(mlastnode) "\";\n";
    t << "          }\n";
    t << "        }\n";
    t << "        folder.style.display = \"block\";\n";
    t << "      }\n";
    t << "    }\n";
    t << "\n";
    t << "    // End script hiding -->        \n";
    t << "    </script>\n";
    t << "  </head>\n";
    t << "\n";
    t << "  <body>\n";
    t << "    <div class=\"directory\">\n";
    t << "      <h3>";
    QCString &projName = Config_getString("PROJECT_NAME");
    if (projName.isEmpty())
    {
      t << "Root";
    }
    else
    {
      t << projName;
    }
    t << "</h3>\n";
    t << "      <div style=\"display: block;\">\n";

    generateTree(t,m_indentNodes[0],0);

    t << "      </div>\n";
    t << "    </div>\n";
    t << "  </body>\n";
    t << "</html>\n";
  }
  generateTreeViewImages();

}

