/*   mv_movie.h  -  declarations for mv_movie.cpp
 *
 *   Copyright (C) 1995-2002 by Boulder Laboratory for 3-Dimensional Electron
 *   Microscopy of Cells ("BL3DEMC") and the Regents of the University of 
 *   Colorado.  See implementation file for full copyright notice.
 *
 *  $Id$
 */                                                                           

#ifndef IMODV_MOVIE_H
#define IMODV_MOVIE_H
#include "imodel.h"
#include <vector>

#define IMODV_MOVIE_FULLAXIS_X -1
#define IMODV_MOVIE_FULLAXIS_Y 1
#define IMODV_MOVIE_START_STATE 0
#define IMODV_MOVIE_END_STATE 1

typedef struct __imodv_struct ImodvApp;

#define CLEAR_RESIZE(a,b,c) { a.clear(); \
  a.swap(std::vector<b>(a)); \
  a.resize(c); }

typedef struct {
  Ipoint rotation;
  Ipoint translate;
  Ipoint clipPoint[IMOD_CLIPSIZE];
  float zoomRad;
  int imgXcenter;
  int imgYcenter;
  int imgZcenter;
  int imgSlices;
  int imgTransparency;
} MovieTerminus;

typedef struct {
  int numFrames;
  int viewNum;
  int fullAxis;
  int numClips;
  int clipFlags;
  Ipoint clipNormal[IMOD_CLIPSIZE];
  std::vector<unsigned char> objStates;
  QString label;
  MovieTerminus start;
  MovieTerminus end;
  int imgAxisFlags;
  int imgWhiteLevel;
  int imgBlackLevel;
  int imgFalseColor;
  int imgXsize;
  int imgYsize;
  int imgZsize;
} MovieSegment;

void imodvMovieFullAxis(int ixy);
void imodvMovieSetStart();
void imodvMovieSetEnd();
void imodvMovieDialog(ImodvApp *a, int state);
void imodvMovieFullAxis();
void imodvMovieUpdate();
void imodvMovieHelp();
void imodvMovieQuit();
int imodvMovieMake();
void imodvMovieStop();
void imodvMovieHelp();
void imodvMovieClosing();
void mvMovieSequenceDialog(ImodvApp *a, int state);
void mvMovieSequenceClosing();
void mvMovieGetSegment(MovieSegment &segment);
void mvMovieSetSegment(MovieSegment &segment);
void mvMovieSetTerminus(int startEnd, MovieSegment &segment);
void mvMovieMontSelection(int mont);
std::vector<MovieSegment> *mvMovieSegmentArray();

#endif
