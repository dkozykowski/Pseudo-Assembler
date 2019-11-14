#include <windows.h>
#include <stdio.h>
#include <string.h>

#define shift 30 //used to move whole right side of terminal to fit it well
#define showContent 1 //if equal to 1 - whole tables' content is leftSideed; if 0 - just indicator to its' first element
#define firstAddress 100 //first of memory addresses

#include "structures.h"
#include "memory.h"
#include "GUI.h"
#include "core.h"
