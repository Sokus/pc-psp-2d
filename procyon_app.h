#ifndef PROCYON_APP_H
#define PROCYON_APP_H

void procyon_app_init();

#endif // PROCYON_APP_H


#ifdef PROCYON_APP_IMPL

#include <stdio.h>

void procyon_app_init()
{
    printf("procyon_app_init()\n");
}

#endif // PROCYON_APP_IMPL