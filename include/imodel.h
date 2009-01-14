/*
 *  imodel.h -- Image model header file.
 *
 *  Original author: James Kremer
 *  Revised by: David Mastronarde   email: mast@colorado.edu
 *
 *  Copyright (C) 1995-2004 by Boulder Laboratory for 3-Dimensional Electron
 *  Microscopy of Cells ("BL3DEMC") and the Regents of the University of 
 *  Colorado.  See dist/COPYRIGHT for full copyright notice.
 *
 *  $Id$
 *  Log at end of file
 */

#ifndef IMODEL_H
#define IMODEL_H
#define IMOD_MODEL_H

#include <stdio.h>
#include <stdlib.h>
#include "imodconfig.h"
#include "hvemtypes.h"

#include "ilist.h"
#include "mrcfiles.h"

/* Mouse mode. */
#define IMOD_MMODEL 1   /* Mouse edits model points.  */
#define IMOD_MMOVIE 2   /* Mouse changes image views. */

/* Imodel Limits */
#define IMOD_STRSIZE 128
#define IMOD_OBJSTRSIZE 64
#define IOBJ_STRSIZE 64
#define IOBJ_EXSIZE 16
#define IMOD_CLIPSIZE  6
#define ANGLE_STRSIZE 32

/* Units for pixel size. */
#define IMOD_UNIT_PIXEL 0
#define IMOD_UNIT_KILO   3
#define IMOD_UNIT_METER  1
#define IMOD_UNIT_CM    -2
#define IMOD_UNIT_MM    -3
#define IMOD_UNIT_UM    -6
#define IMOD_UNIT_NM    -9
#define IMOD_UNIT_ANGSTROM -10
#define IMOD_UNIT_PM    -12

/* Size of circles drawn on points., (Model - drawmode) */
#define IMOD_DRAWMODE_BIG   9
#define IMOD_DRAWMODE_MED   3
#define IMOD_DRAWMODE_SMALL 1

/* Used for reading and writing imod files. */
/*
  #ifdef __vms
  #define MakeID(a,b,c,d) ( (long)(d)<<24L | (long)(c)<<16L | (b)<<8 | (a))
  #else
*/
#define MakeID(a,b,c,d) ((b3dInt32)(a)<<24L | (b3dInt32)(b)<<16L | (c)<<8 | (d))


#define ID_IMOD MakeID('I', 'M', 'O', 'D')  /* imod file                     */
#define ID_OBJT MakeID('O', 'B', 'J', 'T')  /* object header                 */
#define ID_CONT MakeID('C', 'O', 'N', 'T')  /* contour data                  */
#define ID_PNTS MakeID('P', 'N', 'T', 'S')  /* Point data for v01 only */
#define ID_MESH MakeID('M', 'E', 'S', 'H')  /* Mesh data                     */
#define ID_VIEW MakeID('V', 'I', 'E', 'W')  /* 3D View info                  */
#define IMOD_01 MakeID('V', '0', '.', '1')  /* File version 01, imod 1.0     */
#define IMOD_02 MakeID('V', '0', '.', '2')  /* File version 02, imod 1.1     */
#define IMOD_V12 MakeID('V','1', '.', '2')  /* Imod file version 1.2         */
#define ID_VERSION IMOD_V12
#define ID_IEOF MakeID('I', 'E', 'O', 'F')  /* Imod end of file */

/* new data for imod version 1.2, this is an extension to V0.2 files */
#define ID_CLIP MakeID('C', 'L', 'I', 'P')  /* object clip planes. */
#define ID_MCLP MakeID('M', 'C', 'L', 'P') /* Model clip planes */
#define SIZE_CLIP 28

#define ID_IMAT MakeID('I', 'M', 'A', 'T')  /* object material. */
#define SIZE_IMAT 16

#define ID_IMNX MakeID('M', 'I', 'N', 'X')  /* image  nat. transform.*/
#define SIZE_IMNX 72

#define ID_LABL MakeID('L', 'A', 'B', 'L')   
#define ID_OLBL MakeID('O', 'L', 'B', 'L')   

#define ID_SIZE MakeID('S', 'I', 'Z', 'E')  /* size for scattered points */

#define ID_MOST MakeID('M', 'O', 'S', 'T')  /* Model storage items */
#define ID_OBST MakeID('O', 'B', 'S', 'T')  /* Object storage items */
#define ID_COST MakeID('C', 'O', 'S', 'T')  /* Contour storage items */
#define ID_MEST MakeID('M', 'E', 'S', 'T')  /* Mesh storage items */
#define ID_MEPA MakeID('M', 'E', 'P', 'A')  /* Meshing parameters */
#define SIZE_MEPA 76
#define ID_SKLI MakeID('S', 'K', 'L', 'I')  /* Capping skip list for meshing */
#define SIZE_STOR 12
#define ID_SLAN MakeID('S', 'L', 'A', 'N')  /* Slicer angles */
#define SIZE_SLAN 60
#define ID_OGRP MakeID('O', 'G', 'R', 'P')  /* Object group */

/* future data defines. */
#define ID_BRCH MakeID('B', 'R', 'C', 'H')  /* Branch data */

#define IMOD_ERROR_READ    10
#define IMOD_ERROR_WRITE   11
#define IMOD_ERROR_VERSION 20
#define IMOD_ERROR_FORMAT  21
#define IMOD_ERROR_MEMORY  50

/* model flags. */
#define IMODF_FLIPYZ (1l << 16)  /* data is stored with y,z coords flipped. */
#define IMODF_TILTOK (1l << 15)  /* current tilt angles properly stored */
#define IMODF_OTRANS_ORIGIN (1l << 14)  /* otrans has image origin values */
#define IMODF_MAT1_IS_BYTES (1l << 13)  /* mat1 and mat3 are stored as bytes */
#define IMODF_MULTIPLE_CLIP (1l << 12)  /* multiple clip planes possible */

/****************************** Structures ***********************************/

typedef struct Mod_Point
{
  b3dFloat           x;
  b3dFloat           y;
  b3dFloat           z;
}Ipoint;

#ifndef IMODELP_H
typedef struct Mod_Mesh
{
  struct Mod_Point *vert;   /* list of points */
  b3dInt32        *list;    /* index into vert array + instructions */
  b3dInt32        vsize;    /* size of arrays */
  b3dInt32        lsize;
  b3dUInt32       flag;     /* Flags */
  b3dInt16        time;     /* Time value */
  b3dInt16        surf;     /* Surface  */
  Ilist          *store;
}Imesh;
#endif


/* Describes current object, contour and point. */
typedef struct Mod_Index
{
  b3dInt32  object;
  b3dInt32  contour;
  b3dInt32  point;
}Iindex;

/* DOC_SECTION PLANES */
/* DOC_CODE Iplane structure */
/*
 * Holds the parameters for a clipping plane described by
 *    ax + by + cz = d
 */
typedef struct Mod_Plane
{
  b3dFloat a, b, c, d;
} Iplane;
/* END_CODE */

/* DOC_CODE IclipPlanes structure */
/*
 * Holds a set of clipping planes, which may be part of an
 * object, a view, or an object view 
 */
typedef struct Mod_Planes
{
  b3dUByte count;        /* number of clip planes.            */
  b3dUByte flags;        /* Which clip planes are on.         */
  b3dUByte trans;        /* Transparency for clipped area     */
  b3dUByte plane;        /* Current clip plane.               */
  Ipoint normal[IMOD_CLIPSIZE];   /* Normal vector to plane */
  Ipoint point[IMOD_CLIPSIZE];    /* Negative of point in clip plane */
} IclipPlanes;
/* END_CODE */
/* END_SECTION */

/* DOC_SECTION VIEWS */

/* DOC_CODE World flags */
/* Definitions for bits of the Iview world flag */
#define VIEW_WORLD_ON         1         /* Use mat matrix for transformation */
#define VIEW_WORLD_LIGHT      (1l << 1)  /* Use lighting */
#define VIEW_WORLD_DEPTH_CUE  (1l << 2)  /* Enable depth cueing */
#define VIEW_WORLD_WIREFRAME  (1l << 3)  /* Show wireframe not surface */

#define VIEW_WORLD_STEREO   (1l << 4)    /* Stereo flags (Unused 8/21/07) */
#define VIEW_WORLD_HARDWARE (1l << 5)
#define VIEW_WORLD_UPDOWN   (1l << 6)
#define VIEW_WORLD_LOWRES  (1l << 7)

/* Shift and actual bits reserved for point quality (8, 9, 10) */
#define WORLD_QUALITY_SHIFT  8
#define WORLD_QUALITY_BITS   (7l << WORLD_QUALITY_SHIFT)

#define WORLD_KICKOUT_CLIPS (1l << 11)  /* Push Z clipping planes far out */
#define WORLD_MOVE_ALL_CLIP (1l << 12)  /* Move all clipping planes together */
/* END_CODE */

#ifndef IMODELP_H
/* DOC_CODE Iobjview structure */
/* Properties of an object that are stored in a view */
typedef struct Mod_Object_View
{
  b3dUInt32              flags;               /* bit flags IMOD_OBJFLAG        */
  b3dFloat   red;                 /* Red (0 - 1.0)                 */
  b3dFloat   green;               /* Green (0 - 1.0)               */
  b3dFloat   blue;                /* Blue (0 - 1.0)                */
  b3dInt32     pdrawsize;           /* size to draw scattered objs   */

  b3dUByte     linewidth;           /* linewidth in 3-D              */
  b3dUByte     linesty;             /* line draw style               */
  b3dUByte     trans;               /* transperentcy                 */

  /* define clipping planes. max for each object is 7. */
  IclipPlanes clips;

  /* Added info IMAT */
  b3dUByte ambient;   /* Ambient multiplier to color */
  b3dUByte diffuse;   /* Diffuse multiplier to color */
  b3dUByte specular;  /* Specular property, added to color */
  b3dUByte shininess; /* shininess exponent */
  b3dUByte fillred;   /* Fill color red */
  b3dUByte fillgreen; /* Fill color green */
  b3dUByte fillblue;  /* Fill color blue */
  b3dUByte quality;   /* Sphere quality */
  b3dUInt32 mat2;     /* set to 0, use as flags.  Unused */
  b3dUByte valblack;  /* Black level for showing values */
  b3dUByte valwhite;  /* White level for showing values */
  b3dUByte matflags2; /* First two bits: skip low and high end in value draw */
  b3dUByte mat3b3;    /* Unused */
}Iobjview;
/* END_CODE */

/* DOC_CODE Iview structure */
/* Properties of a model view */
typedef struct
{
  /* Set up the camera */
  b3dFloat  fovy;   /* field of view of camera, perspective in degrees.  */
  b3dFloat  rad;    /* viewing radius of sphere encloseing bounding box. */
  b3dFloat  aspect; /* aspect ratio */
  b3dFloat  cnear;  /* clip near range 0.0 to 1.0, default 0.0. */
  b3dFloat  cfar;   /* clip far range 0.0 to 1.0, default 1.0. */
     
  /* Model transformation values for model view. */
  Ipoint rot;
  Ipoint trans;
  Ipoint scale;

  /* World OpenGL transformation matrix (unused 8/21/07). */
  b3dFloat mat[16];

  b3dInt32 world;       /* flags */
  b3dByte  label[32];   /* Label for view */

  b3dFloat dcstart, dcend;  /* Depth cue start and end */
  b3dFloat lightx, lighty;  /* Light position X and Y */
  b3dFloat plax;            /* Parallax angle for stereo */
  IclipPlanes clips;        /* Global clipping planes */
  Iobjview *objview;        /* Array of properties of each object */
  b3dInt32 objvsize;        /* Number of objects properties exist for */
     
}Iview;
/* END_CODE */

/* DOC_CODE IrefImage structure */
/* A structure for keeping track of current and previous image transformation
 * information for a model loaded on an image file */
typedef struct
{
  Ipoint oscale;
  Ipoint otrans;
  Ipoint orot;

  Ipoint cscale;
  Ipoint ctrans;
  Ipoint crot;

}IrefImage;
/* END_CODE */
/* END_SECTION */

/* DOC_SECTION LABELS */
/* DOC_CODE Ilabel structure */
/*
 *  Label Data for Contours and points.
 */
typedef struct
{
  b3dByte *name;     /* The name string for this item */
  b3dInt32  len;     /* Length of name string */
  b3dInt32 index;    /* A value associated with this label */
}IlabelItem;

typedef struct
{
  b3dByte       *name;   /* The name string for the label */
  b3dInt32         len;  /* Length of name */
  b3dInt32        nl;    /* Number of sub-label items */
  IlabelItem *label;     /* Array of items */
}Ilabel;
/* END_CODE */
/* END_SECTION */
#endif

/* DOC_SECTION MESHPARAMS */
/* DOC_CODE MeshParams structure */
/*
 * Parameters for meshing an object 
 */
typedef struct Meshing_Param
{
  b3dUInt32 flags;         /* IMESH_MK_* flags */
  b3dInt32 cap;            /* Capping parameter */
  b3dInt32 passes;         /* Number of passes for skipped sections */
  b3dInt32 capSkipNz;      /* Number of Z values not to cap to */
  b3dInt32 inczLowRes;     /* Z increment for low res mesh */
  b3dInt32 inczHighRes;    /* Z increment for high res mesh */
  b3dInt32 minz, maxz;     /* Starting and ending Z to mesh */
  b3dInt32 spareInt;
  b3dFloat overlap;        /* Fractional overlap required if nonzero */
  b3dFloat tubeDiameter;   /* Diameter when meshing tubes */
  b3dFloat xmin, xmax;     /* X and Y limits for triangle output */
  b3dFloat ymin, ymax;
  b3dFloat tolLowRes;      /* Point reduction tolerance for low and high res */
  b3dFloat tolHighRes;
  b3dFloat flatCrit;       /* Criterion Z difference for rotating contours */
  b3dFloat spareFloat;
  b3dInt32 *capSkipZlist;  /* List of Z values not to cap to */
} MeshParams;
/* END_CODE */
/* END_SECTION */

#ifndef IMODELP_H
/*
 * Contours are an array of points describing an open or closed contour.
 * The points can also be unconnected or the points can be in pairs
 * describing scan lines.
 */
typedef struct Mod_Contour
{
  /* Run time data */
  Ipoint       *pts;    /* Points data.                      */
  b3dFloat     *sizes;  /* sizes for scattered points        */

  /* data below is written to file */
  b3dInt32     psize;  /* Number of points.                 */
  b3dUInt32    flags;  /* Default 0 means use object flags. */
  b3dInt32     time;   /* Time index.                       */
  b3dInt32     surf;   /* Surface number.                   */
  Ilabel      *label;

  Ilist       *store;
}Icont;


/* An Object is an array of contours */
typedef struct Mod_Object
{
  /* Run time data */
  struct Mod_Contour *cont;              /* Contour data.                 */
  struct Mod_Mesh    *mesh;

  b3dUInt32 fgcolor;  /* colors used for colorindex rendering. */
  b3dUInt32 bgcolor;

  /* data below is written to file */
  b3dByte      name[IOBJ_STRSIZE];  /* Name of Object.              */
  b3dUInt32    extra[IOBJ_EXSIZE];  /* extra unused data = 0        */

  /* Use some EXSIZE data for light location. 
     b3dFloat            lightOrientation[3];
  */


  b3dInt32     contsize;            /* Number of Contours in object. */
  b3dUInt32    flags;               /* bit flags IMOD_OBJFLAG        */
  b3dInt32     axis;                /* Z = 0, X = 1, Y = 2. (future) */
  b3dInt32     drawmode;            /* Tells type of points to draw  */
  /* for scatterd objects.         */
  b3dFloat     red;                 /* Red (0 - 1.0)                 */
  b3dFloat     green;               /* Green (0 - 1.0)               */
  b3dFloat     blue;                /* Blue (0 - 1.0)                */
  b3dInt32     pdrawsize;           /* size to draw scattered objs   */

  b3dUByte     symbol;              /* default 0=circle              */
  b3dUByte     symsize;             /* size of symbol.               */
  b3dUByte     linewidth2;          /* linewidth in 2-D              */
  b3dUByte     linewidth;           /* linewidth in 3-D              */
  b3dUByte     linesty;             /* line draw style               */
  b3dUByte     symflags;
  b3dUByte     sympad;
  b3dUByte     trans;               /* transparency                  */

  b3dInt32     meshsize;            /* Number of meshes in object.   */
  b3dInt32     surfsize;            /* Max surfaces in object.       */

  /* new extended data imod 1.2 */
  /* chunk CLIP */
  /* define clipping planes. max for each object is 7. */
  IclipPlanes clips;

  /* Added info IMAT */
  b3dUByte ambient;   /* Ambient multiplier to color */
  b3dUByte diffuse;   /* Diffuse multiplier to color */
  b3dUByte specular;  /* Specular property, added to color */
  b3dUByte shininess; /* shininess exponent */
  b3dUByte fillred;   /* Fill color red */
  b3dUByte fillgreen; /* Fill color green */
  b3dUByte fillblue;  /* Fill color blue */
  b3dUByte quality;   /* Sphere quality */
  b3dUInt32 mat2;     /* set to 0, use as flags.  Unused */
  b3dUByte valblack;  /* Black level for showing values */
  b3dUByte valwhite;  /* White level for showing values */
  b3dUByte matflags2; /* First two bits: skip low and high end in value draw */
  b3dUByte mat3b3;    /* Unused */

  Ilabel *label;      /* Labels for surfaces */
  MeshParams *meshParam; /* Meshing parameters */
  Ilist  *store;
}Iobj;



/* A Model is an array of objects */
typedef struct Mod_Model
{
  /* Run time data */
  Iobj  *obj;          /* Object data.                       */
  FILE  *file;         /* Current file handle.               */
  int   ctime;         /* current time index.                */

  /* data written to file  */
  b3dByte   name[IMOD_STRSIZE];        /* file name of model.                */
                                       /* Use labels for model name.         */
  b3dInt32    xmax, ymax, zmax;  
  b3dInt32    objsize;                 /* Number of objects in model.        */
  b3dUInt32   flags;                   /* IMODF_FLAG values                  */
  b3dInt32    drawmode;                /* 1 to draw model, -1 not to         */
  b3dInt32    mousemode;               /* Tells what mouse buttons do.       */
  b3dInt32    blacklevel, whitelevel;  /* Contrast adjustment.               */
  b3dFloat  xoffset, yoffset, zoffset; /* offsets used for 3-D display       */
  b3dFloat  xscale;                    /* pixel size zoom.                   */
  b3dFloat  yscale;                    /* y aspect for pixels.               */
  b3dFloat  zscale;                    /* z aspect for sections.             */
  Iindex cindex;              /* Current object, contour & point     */
  b3dInt32    res;            /* Number of pixels between points.    */
  b3dInt32    thresh;         /* Threshold level for auto cont.      */
  b3dFloat  pixsize;          /* size of unit 1.0 pixel              */
  b3dInt32    units;          /* units of given size.                */
  b3dInt32    csum;           /* checksum storage.                   */
  b3dInt32    cview;
  b3dInt32    viewsize;     
  b3dInt32    tmax;           /* max time */
     
  /* imod file version 0.2 data */
  b3dFloat  alpha, beta, gamma;        /* rotation of model in 3-D view. */

  /* more runtime data */
  b3dUInt16  color;      /* color of current object */
  Iview      *view;      /* Array of view data */
  int        editGlobalClip;   /* Flag that global clip is selected */
  int        curObjGroup;  /* Current object group */

  IrefImage  *refImage;
  char       *fileName;
  int        xybin;       /* Binning in X and Y */
  int        zbin;        /* Binning in Z */

  /* Optional data written to file */
  Ilist  *store;          /* General storage data */
  Ilist  *slicerAng;       /* Slicer angles */
  Ilist  *groupList;      /* Object group lists */
}Imod;




/* Used for drawing models, imodv */
typedef struct Mod_Draw
{
  b3dInt32 llx;      /* Used for drawing 2d models to sub area */
  b3dInt32 lly;     
  b3dInt32 urx;
  b3dInt32 ury;
  b3dInt32 llz;
  b3dInt32 urz;
  b3dFloat xorg;     /* Offset for 2d, translation for 3d models. */
  b3dFloat yorg;
  b3dFloat zorg;
  b3dInt32 xrot;     /* Amount to rotate model */
  b3dInt32 yrot;
  b3dInt32 zrot;
  b3dFloat xtrans;   /* Amount to translate model */
  b3dFloat ytrans;
  b3dFloat ztrans;
  b3dInt32 axis;     /* Axis to rotate.         */
  b3dInt32 cmapbase; /* Where to start colormap */
  b3dFloat left,
    right,
    bottom,
    top;
  b3dInt32    cnear, /* Clipping planes */
    cfar;
  b3dInt32   dc;     /* min depth cue factor  range x/1 to x/16*/
  b3dFloat zoom;     /* Mag factor for displaying */
  b3dFloat cdist;    /* Distence for center of model to far corner */
  b3dFloat edist;    /* Distence from center of model to eye.      */
  b3dInt32 xrotm;    /* xyz movie amounts */
  b3dInt32 yrotm;
  b3dInt32 zrotm;
  b3dInt32 arot;     /* amount of change in rotation */
  b3dFloat azoom;
  b3dFloat atrans;
  b3dFloat azscale;
} Idraw;


typedef struct Mod_Transform
{
  b3dFloat x;       /* input x, y and z. */
  b3dFloat y;
  b3dFloat z;
  b3dFloat xout;    /* output values after transformation. */
  b3dFloat yout;
  b3dFloat zout;
  b3dFloat xtrans;  /* amount to translate x, y and z. */
  b3dFloat ytrans;
  b3dFloat ztrans;
  b3dFloat xrot;    /* amount to rotate x, y, and z. */
  b3dFloat yrot;
  b3dFloat zrot;
  b3dFloat xscale;  /* amount to scale x, y and z. */
  b3dFloat yscale;
  b3dFloat zscale;
}Itrans;

typedef struct slicer_angles
{
  b3dInt32 time;
  b3dFloat angles[3];
  Ipoint center;
  char label[ANGLE_STRSIZE];
} SlicerAngles;
#endif

/* include functions in iobj.c, icont.c, imesh.c, ipoint.c */
#include "iobj.h"
#include "icont.h"
#include "imesh.h"
#include "ipoint.h"
#include "imat.h"
#include "iplane.h"
#include "iview.h"
#include "istore.h"
#include "b3dutil.h"

/*****************************************************************************/
/* imodel.c functions                                                        */
/* Model structure is passed to functions.                                   */
/*****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

  Imod *imodNew       (void);
  void  imodDelete(Imod *imod);
  int   imodNewObject (Imod *imod);
  int   imodDeleteObject(Imod *imod, int index);
  int   imodNextObject(Imod *imod);
  int   imodPrevObject(Imod *imod);
  Iobj *imodObjectGet(Imod *imod);
  Iobj *imodObjectGetFirst(Imod *imod);
  Iobj *imodObjectGetNext(Imod *imod);
  int   imodMoveObject(Imod *imod, int obOld, int obNew);
  int   imodDefault(Imod *imod);
  void  imodCleanSurf(Imod *imod);
  void  imodFlipYZ(Imod *imod);
  int imodSetRefImage(Imod *imod, MrcHeader *hdata);
  int imodTransFromRefImage(Imod *imod, IrefImage *iref, Ipoint binScale);
  void imodTransFromMats(Imod *imod, Imat *mat, Imat *matNorm, Imat *matClip);

  int   imodNewContour(Imod *imod);
  int   imodPrevContour(Imod *imod);
  int   imodNextContour(Imod *imod);
  Icont *imodContourGet(Imod *mod);
  Icont *imodContourGetFirst(Imod *imod);
  Icont *imodContourGetNext(Imod *imod);
  int imodDeleteContour(Imod *imod, int index);
  void imodDelCurrentContour(Imod *imod);

  int   imodNewPoint(Imod *imod, Ipoint *pt);
  int   imodInsertPoint(Imod *imod, Ipoint *point, int index);
  int   imodDeletePoint(Imod *imod);
  int   imodPrevPoint(Imod *imod);
  int   imodNextPoint(Imod *imod);
  Ipoint *imodPointGet(Imod *imod);
  Ipoint *imodPointGetFirst(Imod *imod);
  Ipoint *imodPointGetNext(Imod *imod);


  void  imodGetIndex(Imod *imod, int *object, int *contour, int *point);
  void  imodSetIndex(Imod *imod, int object, int contour, int point);
  void  imodGetBoundingBox(Imod *imod, Ipoint *min, Ipoint *max);
  int   imodGetMaxObject(Imod *imod);
  float imodGetZScale(Imod *imod);
  float imodGetPixelSize(Imod *imod);
  int   imodGetFlipped(Imod *imod);
  int   imodTransform(Imod *imod, Imat *mat);

  double  imodel_dist(Imod *imod);
  void    imodel_minpt(Imod *imod, Ipoint *pnt);
  void    imodel_maxpt(Imod *imod, Ipoint *pnt);
  int     imodGetMaxTime(Imod *imod);
  char   *imodUnits(Imod *mod);
  Ipoint *imodNearestPoint(Imod *imod, Ipoint *pt);
  int    imodChecksum(Imod *imod);
  int     imodel_lock(Imod *mod, int flag);
  int     imodel_unlock(Imod *mod);
  int     imodel_transform_slice(Imod *model, float *mat, 
                                 int slice);
  int     imodel_transform(struct Mod_Transform *tr);
  int     imodel_model_clean(Imod *mod, int keepEmptyObjs);

  /***************************************************************************/
  /* imodel_from.c functions                                                 */
  /***************************************************************************/
  Imod *imod_from_wmod(FILE *fin);
  Imod *imod_from_synu(FILE *fin);
  Imod *imod_from_srec(FILE *fin);
  int substr(char bs[], char ls[]);
  int imodel_from_fgetline(FILE *fp, char s[],int limit);

  /***************************************************************************/
  /* imodel_to.c functions                                                   */
  /***************************************************************************/
  int imod_to_nff(Imod  *mod, FILE *fout);
  int imod_to_wmod(Imod *mod, FILE *fout, const char *filename);
  int imod_to_synu(Imod *mod);

  /***************************************************************************/
  /* imodel_files.c functions                                                */
  /***************************************************************************/
  Imod *imodFileRead(const char *filename);
  int   imodFileWrite(Imod *imod, const char *filename);

  int   imodOpenFile  (const char *filename, const char *mode, Imod *imod);
  int   imodCloseFile (Imod *imod);
  int   imodReadFile  (Imod *imod);
  Imod *imodRead      (const char *filename);
  int   imodWriteFile (Imod *imod);
  int   imodWrite     (Imod *imod, FILE *fout);
  int   imodWriteAscii(Imod *imod);
  int   imodReadAscii (Imod *imod);
  int   imodFgetline(FILE *fp, char s[],int limit);


  float imodGetFloat(FILE *fp);
  int   imodGetFloats(FILE *fp, float *buf, int size);
  int   imodPutFloats(FILE *fp, float *buf, int size);
  int   imodPutScaledPoints(FILE *fp, Ipoint *buf, int size, Ipoint *scale);
  int   imodPutFloat(FILE *fp, float *dat);
  int   imodGetInt(FILE *fp);
  int   imodGetInts(FILE *fp, void *buf, int size);
  int   imodPutInts(FILE *fp, void *buf, int size);
  int   imodPutInt(FILE *fp,  void *dat);
  b3dInt16 imodGetShort(FILE *fp);
  int   imodPutShort(FILE *fp, void *buf);
  unsigned char  imodGetByte(FILE *fp);
  int   imodGetBytes(FILE *fp, unsigned char *buf, int size);
  int   imodPutBytes(FILE *fp, unsigned char *buf, int size);
  int   imodPutByte(FILE *fp,  unsigned char *dat);
  void  imodFromVmsFloats(unsigned char *data, int amt);

  /***************************************************************************/
  /* ilabel.c functions                                                      */
  /***************************************************************************/
  Ilabel *imodLabelNew(void);
  void    imodLabelDelete(Ilabel *label);
  Ilabel *imodLabelDup(Ilabel *label);
  void    imodLabelName(Ilabel *label, const char *val);
  void    imodLabelItemAdd(Ilabel *label, const char *val, int index);
  void    imodLabelItemMove(Ilabel *label, int to_index, int from_index);
  void    imodLabelItemDelete(Ilabel *label, int index);
  char   *imodLabelItemGet(Ilabel *label, int index);
  void    imodLabelPrint(Ilabel *lab, FILE *fout);
  int     imodLabelWrite(Ilabel *lab, b3dUInt32 tag, FILE *fout);
  Ilabel *imodLabelRead(FILE *fin, int *err);
  int     imodLabelMatch(Ilabel *label, char *tstr);
  int     imodLabelItemMatch(Ilabel *label, char *tstr, int index);
  int     ilabelMatchReg(char *exp, char *str);

#ifdef __cplusplus
}
#endif

/*    
    $Log$
    Revision 3.44  2009/01/02 05:19:19  mast
    const char * for Qt 4 port

    Revision 3.43  2008/01/27 06:25:36  mast
    Added object group list

    Revision 3.42  2007/12/04 18:27:35  mast
    Exposed Ipoint definition for plugins

    Revision 3.41  2007/11/28 01:30:31  mast
    Added conditional sections so it can be used instead of model.h

    Revision 3.40  2007/09/25 15:45:05  mast
    Added ref image function

    Revision 3.39  2007/09/22 00:05:28  mast
    Renamed mat3b2 to matflags2, removed lock and redraw variables

    Revision 3.38  2007/08/26 06:56:34  mast
    Documentation changes

    Revision 3.37  2007/06/22 04:58:54  mast
    Doc changes

    Revision 3.36  2007/05/25 05:18:11  mast
    Changes for slicer angle storage

    Revision 3.35  2006/11/02 07:15:15  mast
    Added doc tags

    Revision 3.34  2006/09/12 15:18:18  mast
    Various meshing changes, and new meshparam structure

    Revision 3.33  2006/08/31 21:04:00  mast
    New flags and redefinition of mat1 and mat3 variables

    Revision 3.32  2006/06/26 14:49:09  mast
    Moved miscellaneous functions to b3dutil

    Revision 3.31  2006/06/18 19:35:14  mast
    Changed function type in amoeba

    Revision 3.30  2006/06/06 16:25:06  mast
    Added amoeba declarations

    Revision 3.29  2005/12/11 18:24:36  mast
    Added world flag for kicking extreme clipping planes farther out

    Revision 3.28  2005/10/16 20:25:40  mast
    Split transformation function into two

    Revision 3.27  2005/10/13 20:01:41  mast
    Added model transform function

    Revision 3.26  2005/06/29 05:34:20  mast
    Added istore include

    Revision 3.25  2005/06/20 22:23:15  mast
    Changes for new istore

    Revision 3.24  2005/04/23 23:36:23  mast
    Moved some functions into imodel.c

    Revision 3.23  2005/03/22 16:46:26  mast
    Fixed return type of imodDeleteObject

    Revision 3.22  2005/03/20 19:55:48  mast
    Eliminating duplicate functions

    Revision 3.21  2004/12/06 22:00:08  mast
    Removed DelPoint

    Revision 3.20  2004/11/20 04:05:26  mast
    removed virtual stuff, added contour store and function to move object

    Revision 3.19  2004/11/05 18:52:53  mast
    Include local files with quotes, not brackets

    Revision 3.18  2004/09/28 22:21:59  mast
    Added parselist declaration

    Revision 3.17  2004/09/21 20:08:04  mast
    Defined new clipping structures

    Revision 3.16  2004/09/17 19:48:19  mast
    Document use of mat1b3 for point quality

    Revision 3.15  2004/04/28 05:30:00  mast
    defined world flags with bit shifts

    Revision 3.14  2004/01/05 17:22:25  mast
    Added binning run-time member of model structure

    Revision 3.13  2003/10/24 04:07:51  mast
    move some functions to new b3dutil

    Revision 3.12  2003/09/16 02:05:53  mast
    Add function to return flipping state of model

    Revision 3.11  2003/06/27 20:07:52  mast
    Defined bits in world flag for quality;
    changed imodChecksum return type from long to int

    Revision 3.10  2003/03/03 22:45:31  mast
    Changes sizes and a few other variables from unsigned to signed ints

    Revision 3.9  2003/02/21 22:17:47  mast
    Implement new b3d types

    Revision 3.8  2002/12/01 15:40:44  mast
    move ilabel declarations so they are inside the extern "C" construct

    Revision 3.7  2002/09/04 23:13:10  mast
    Add a flag to indicate mat and mat3 are stored as bytes

    Revision 3.6  2002/09/03 20:06:14  mast
    Changed mat1 and mat3 in object this time; last time was in object view

    Revision 3.5  2002/09/03 19:37:01  mast
    Redefine object materials mat1 and mat3 from UINT to 4 UBYTES each because
    code accesses them as individual bytes in numbered positions

    Revision 3.4  2002/08/03 22:46:56  mast
    Changed some comments

    Revision 3.3  2002/07/20 23:26:44  mast
    Define a flag to signify that refImage.otrans has image origin info

    Revision 3.2  2002/05/20 15:27:15  mast
    Fix documentation of contour type and surf

    Revision 3.1  2001/12/05 16:02:06  mast
    Add declaration of imodFromVmsFloats in imodel_files.c

*/
#endif /* imodel.h */
