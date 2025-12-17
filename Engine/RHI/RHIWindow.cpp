//
// Created by Admin on 2025/12/17.
//

#include "RHIWindow.h"

void FRHIWindow::Open() {}

void FRHIWindow::Close() {}

void FRHIWindow::Destroy() {}

FRHIWindow::~FRHIWindow()
{
    if (IsOpened())
    {
        Close();
    }
    Destroy();
}