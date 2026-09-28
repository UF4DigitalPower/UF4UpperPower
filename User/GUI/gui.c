/**
  ******************************************************************************
  * @file    gui.c
  * @brief   Public GUI compatibility facade.
  ******************************************************************************
  */
#include "gui_internal.h"

void DrawINArea(void)
{
  draw_home_page();
}

void DrawOUTArea(void)
{
  draw_status_bar();
}

void DrawChartArea(void)
{
  draw_content();
}
