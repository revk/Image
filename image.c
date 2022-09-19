// Image handling tools
// Copyright (c) 2001 Adrian Kennard
// This software is provided under the terms of the GPL v2 or later.
// This software is provided free of charge with a full "Money back" guarantee.
// Use entirely at your own risk. We accept no liability. If you don't like that - don't use it.

// Intended for use in applications that need to make images for web pages on the fly


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include "image.h"

#define	INTERLACE
#define CLEAR
#define USEZLIB

#ifdef USEZLIB
#include <zlib.h>
#endif

static unsigned char const teletext[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,        //  
   0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x20, 0x00, 0x00,        // !
   0x50, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,        // "
   0x50, 0xf8, 0x50, 0x50, 0x50, 0xf8, 0x50, 0x00, 0x00,        // #
   0x20, 0x78, 0xa0, 0x70, 0x28, 0xf0, 0x20, 0x00, 0x00,        // $
   0xc0, 0xc8, 0x10, 0x20, 0x40, 0x98, 0x18, 0x00, 0x00,        // %
   0x40, 0xa0, 0x40, 0xa0, 0xa8, 0x90, 0x68, 0x00, 0x00,        // &
   0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,        // '
   0x10, 0x20, 0x40, 0x40, 0x40, 0x20, 0x10, 0x00, 0x00,        // (
   0x40, 0x20, 0x10, 0x10, 0x10, 0x20, 0x40, 0x00, 0x00,        // )
   0x20, 0xa8, 0x70, 0xf8, 0x70, 0xa8, 0x20, 0x00, 0x00,        // *
   0x00, 0x20, 0x20, 0xf8, 0x20, 0x20, 0x00, 0x00, 0x00,        // +
   0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x20, 0x40, 0x00,        // ,
   0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,        // -
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00,        // .
   0x00, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00, 0x00, 0x00,        // /
   0x70, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00,        // 0 (non crossed)
   0x20, 0x60, 0xa0, 0x20, 0x20, 0x20, 0xf8, 0x00, 0x00,        // 1
   0x70, 0x88, 0x08, 0x30, 0x40, 0x80, 0xf8, 0x00, 0x00,        // 2
   0x70, 0x88, 0x08, 0x30, 0x08, 0x88, 0x70, 0x00, 0x00,        // 3
   0x10, 0x30, 0x50, 0x90, 0xf8, 0x10, 0x10, 0x00, 0x00,        // 4
   0xf8, 0x80, 0xf0, 0x08, 0x08, 0x88, 0x70, 0x00, 0x00,        // 5
   0x70, 0x88, 0x80, 0xf0, 0x88, 0x88, 0x70, 0x00, 0x00,        // 6
   0xf8, 0x08, 0x08, 0x10, 0x20, 0x40, 0x40, 0x00, 0x00,        // 7
   0x70, 0x88, 0x88, 0x70, 0x88, 0x88, 0x70, 0x00, 0x00,        // 8
   0x70, 0x88, 0x88, 0x78, 0x08, 0x88, 0x70, 0x00, 0x00,        // 9
   0x00, 0x00, 0x00, 0x20, 0x00, 0x20, 0x00, 0x00, 0x00,        // :
   0x00, 0x00, 0x00, 0x20, 0x00, 0x20, 0x20, 0x40, 0x00,        // ;
   0x08, 0x10, 0x20, 0x40, 0x20, 0x10, 0x08, 0x00, 0x00,        // <
   0x00, 0x00, 0xf8, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00,        // =
   0x80, 0x40, 0x20, 0x10, 0x20, 0x40, 0x80, 0x00, 0x00,        // >
   0x70, 0x88, 0x08, 0x30, 0x20, 0x00, 0x20, 0x00, 0x00,        // ?
   0x70, 0x88, 0xb8, 0xa8, 0xb8, 0x80, 0x78, 0x00, 0x00,        // @
   0x70, 0x88, 0x88, 0xf8, 0x88, 0x88, 0x88, 0x00, 0x00,        // A
   0xf0, 0x88, 0x88, 0xf0, 0x88, 0x88, 0xf0, 0x00, 0x00,        // B
   0x70, 0x88, 0x80, 0x80, 0x80, 0x88, 0x70, 0x00, 0x00,        // C
   0xf0, 0x88, 0x88, 0x88, 0x88, 0x88, 0xf0, 0x00, 0x00,        // D
   0xf8, 0x80, 0x80, 0xf0, 0x80, 0x80, 0xf8, 0x00, 0x00,        // E
   0xf8, 0x80, 0x80, 0xf0, 0x80, 0x80, 0x80, 0x00, 0x00,        // F
   0x70, 0x88, 0x80, 0xb8, 0x88, 0x88, 0x78, 0x00, 0x00,        // G
   0x88, 0x88, 0x88, 0xf8, 0x88, 0x88, 0x88, 0x00, 0x00,        // H
   0xf8, 0x20, 0x20, 0x20, 0x20, 0x20, 0xf8, 0x00, 0x00,        // I
   0xf8, 0x20, 0x20, 0x20, 0x20, 0x20, 0xc0, 0x00, 0x00,        // J
   0x88, 0x90, 0xa0, 0xc0, 0xa0, 0x90, 0x88, 0x00, 0x00,        // K
   0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xf8, 0x00, 0x00,        // L
   0x88, 0xd8, 0xa8, 0x88, 0x88, 0x88, 0x88, 0x00, 0x00,        // M
   0x88, 0xc8, 0xa8, 0xa8, 0xa8, 0x98, 0x88, 0x00, 0x00,        // N
   0x70, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00,        // O
   0xf0, 0x88, 0x88, 0xf0, 0x80, 0x80, 0x80, 0x00, 0x00,        // P
   0x70, 0x88, 0x88, 0x88, 0xa8, 0x90, 0x68, 0x00, 0x00,        // Q
   0xf0, 0x88, 0x88, 0xf0, 0xa0, 0x90, 0x88, 0x00, 0x00,        // R
   0x70, 0x88, 0x80, 0x70, 0x08, 0x88, 0x70, 0x00, 0x00,        // S
   0xf8, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00,        // T
   0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00,        // U
   0x88, 0x88, 0x88, 0x88, 0x50, 0x50, 0x20, 0x00, 0x00,        // V
   0x88, 0x88, 0x88, 0x88, 0xa8, 0xa8, 0x50, 0x00, 0x00,        // W
   0x88, 0x88, 0x50, 0x20, 0x50, 0x88, 0x88, 0x00, 0x00,        // X
   0x88, 0x88, 0x50, 0x50, 0x20, 0x20, 0x20, 0x00, 0x00,        // Y
   0xf8, 0x08, 0x10, 0x20, 0x40, 0x80, 0xf8, 0x00, 0x00,        // Z
   0x70, 0x40, 0x40, 0x40, 0x40, 0x40, 0x70, 0x00, 0x00,        // [
   0x00, 0x80, 0x40, 0x20, 0x10, 0x08, 0x00, 0x00, 0x00,        // backslash
   0x70, 0x10, 0x10, 0x10, 0x10, 0x10, 0x70, 0x00, 0x00,        // ]
   0x20, 0x50, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,        // ^
#if 0
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8,        // _
#else
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,        // _ (space)
#endif
   0x40, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,        // `
   0x00, 0x00, 0x78, 0x88, 0x88, 0x98, 0x68, 0x00, 0x00,        // a
   0x80, 0x80, 0xf0, 0x88, 0x88, 0x88, 0xf0, 0x00, 0x00,        // b
   0x00, 0x00, 0x70, 0x88, 0x80, 0x88, 0x70, 0x00, 0x00,        // c
   0x08, 0x08, 0x78, 0x88, 0x88, 0x88, 0x78, 0x00, 0x00,        // d
   0x00, 0x00, 0x70, 0x88, 0xf8, 0x80, 0x78, 0x00, 0x00,        // e
   0x18, 0x20, 0x20, 0xf8, 0x20, 0x20, 0x20, 0x00, 0x00,        // f
   0x00, 0x00, 0x78, 0x88, 0x88, 0x88, 0x78, 0x08, 0x70,        // g
   0x80, 0x80, 0xf0, 0x88, 0x88, 0x88, 0x88, 0x00, 0x00,        // h
   0x20, 0x00, 0x60, 0x20, 0x20, 0x20, 0x70, 0x00, 0x00,        // i
   0x20, 0x00, 0x60, 0x20, 0x20, 0x20, 0x20, 0x20, 0xc0,        // j
   0x80, 0x80, 0x88, 0x90, 0xe0, 0x90, 0x88, 0x00, 0x00,        // k
   0x60, 0x20, 0x20, 0x20, 0x20, 0x20, 0x70, 0x00, 0x00,        // l
   0x00, 0x00, 0xd0, 0xa8, 0xa8, 0xa8, 0xa8, 0x00, 0x00,        // m
   0x00, 0x00, 0xf0, 0x88, 0x88, 0x88, 0x88, 0x00, 0x00,        // n
   0x00, 0x00, 0x70, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00,        // o
   0x00, 0x00, 0xf0, 0x88, 0x88, 0x88, 0xf0, 0x80, 0x80,        // p
   0x00, 0x00, 0x78, 0x88, 0x88, 0x88, 0x78, 0x08, 0x08,        // q
   0x00, 0x00, 0xf0, 0x88, 0x80, 0x80, 0x80, 0x00, 0x00,        // r
   0x00, 0x00, 0x78, 0x80, 0x70, 0x08, 0xf0, 0x00, 0x00,        // s
   0x40, 0x40, 0xe0, 0x40, 0x40, 0x48, 0x30, 0x00, 0x00,        // t
   0x00, 0x00, 0x88, 0x88, 0x88, 0x88, 0x78, 0x00, 0x00,        // u
   0x00, 0x00, 0x88, 0x88, 0x50, 0x50, 0x20, 0x00, 0x00,        // v
   0x00, 0x00, 0x88, 0x88, 0xa8, 0xa8, 0x50, 0x00, 0x00,        // w
   0x00, 0x00, 0x88, 0x50, 0x20, 0x50, 0x88, 0x00, 0x00,        // x
   0x00, 0x00, 0x88, 0x88, 0x88, 0x88, 0x78, 0x08, 0x70,        // y
   0x00, 0x00, 0xf8, 0x10, 0x20, 0x40, 0xf8, 0x00, 0x00,        // z
   0x18, 0x20, 0x20, 0x40, 0x20, 0x20, 0x18, 0x00, 0x00,        // {
   0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00,        // |
   0xc0, 0x20, 0x20, 0x10, 0x20, 0x20, 0xc0, 0x00, 0x00,        // }
   0x40, 0xa8, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,        // ~
   0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0x00, 0x00,        //
};

static uint8_t const bbc[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      //  
   0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x00,      // !
   0x6C, 0x6C, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00,      // "
   0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00,      // #
   0x0C, 0x3F, 0x68, 0x3E, 0x0B, 0x7E, 0x18, 0x00,      // $
   0x60, 0x66, 0x0C, 0x18, 0x30, 0x66, 0x06, 0x00,      // %
   0x38, 0x6C, 0x6C, 0x38, 0x6D, 0x66, 0x3B, 0x00,      // &
   0x0C, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00,      // '
   0x0C, 0x18, 0x30, 0x30, 0x30, 0x18, 0x0C, 0x00,      // (
   0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x18, 0x30, 0x00,      // )
   0x00, 0x18, 0x7E, 0x3C, 0x7E, 0x18, 0x00, 0x00,      // *
   0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x00,      // +
   0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30,      // ,
   0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00,      // -
   0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00,      // .
   0x00, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x00, 0x00,      // /
   0x18, 0x24, 0x66, 0x66, 0x66, 0x24, 0x18, 0x00,      // 0 (non crossed)
   0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00,      // 1
   0x3C, 0x66, 0x06, 0x0C, 0x18, 0x30, 0x7E, 0x00,      // 2
   0x3C, 0x66, 0x06, 0x1C, 0x06, 0x66, 0x3C, 0x00,      // 3
   0x0C, 0x1C, 0x3C, 0x6C, 0x7E, 0x0C, 0x0C, 0x00,      // 4
   0x7E, 0x60, 0x7C, 0x06, 0x06, 0x66, 0x3C, 0x00,      // 5
   0x1C, 0x30, 0x60, 0x7C, 0x66, 0x66, 0x3C, 0x00,      // 6
   0x7E, 0x06, 0x0C, 0x18, 0x30, 0x30, 0x30, 0x00,      // 7
   0x3C, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x3C, 0x00,      // 8
   0x3C, 0x66, 0x66, 0x3E, 0x06, 0x0C, 0x38, 0x00,      // 9
   0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00,      // :
   0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x30,      // ;
   0x0C, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0C, 0x00,      // <
   0x00, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0x00, 0x00,      // =
   0x30, 0x18, 0x0C, 0x06, 0x0C, 0x18, 0x30, 0x00,      // >
   0x3C, 0x66, 0x0C, 0x18, 0x18, 0x00, 0x18, 0x00,      // ?
   0x3C, 0x66, 0x6E, 0x6A, 0x6E, 0x60, 0x3C, 0x00,      // @
   0x3C, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00,      // A
   0x7C, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x7C, 0x00,      // B
   0x3C, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3C, 0x00,      // C
   0x78, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0x78, 0x00,      // D
   0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x7E, 0x00,      // E
   0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x60, 0x00,      // F
   0x3C, 0x66, 0x60, 0x6E, 0x66, 0x66, 0x3C, 0x00,      // G
   0x66, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00,      // H
   0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00,      // I
   0x3E, 0x0C, 0x0C, 0x0C, 0x0C, 0x6C, 0x38, 0x00,      // J
   0x66, 0x6C, 0x78, 0x70, 0x78, 0x6C, 0x66, 0x00,      // K
   0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7E, 0x00,      // L
   0x63, 0x77, 0x7F, 0x6B, 0x6B, 0x63, 0x63, 0x00,      // M
   0x66, 0x66, 0x76, 0x7E, 0x6E, 0x66, 0x66, 0x00,      // N
   0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00,      // O
   0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x60, 0x00,      // P
   0x3C, 0x66, 0x66, 0x66, 0x6A, 0x6C, 0x36, 0x00,      // Q
   0x7C, 0x66, 0x66, 0x7C, 0x6C, 0x66, 0x66, 0x00,      // R
   0x3C, 0x66, 0x60, 0x3C, 0x06, 0x66, 0x3C, 0x00,      // S
   0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00,      // T
   0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00,      // U
   0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00,      // V
   0x63, 0x63, 0x6B, 0x6B, 0x7F, 0x77, 0x63, 0x00,      // W
   0x66, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0x66, 0x00,      // X
   0x66, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x18, 0x00,      // Y
   0x7E, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x7E, 0x00,      // Z
   0x7C, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7C, 0x00,      // [
   0x00, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x00, 0x00,      // 
   0x3E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x3E, 0x00,      // ]
   0x18, 0x3C, 0x66, 0x42, 0x00, 0x00, 0x00, 0x00,      // ^
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,      // _
   0x1C, 0x36, 0x30, 0x7C, 0x30, 0x30, 0x7E, 0x00,      // `
   0x00, 0x00, 0x3C, 0x06, 0x3E, 0x66, 0x3E, 0x00,      // a
   0x60, 0x60, 0x7C, 0x66, 0x66, 0x66, 0x7C, 0x00,      // b
   0x00, 0x00, 0x3C, 0x66, 0x60, 0x66, 0x3C, 0x00,      // c
   0x06, 0x06, 0x3E, 0x66, 0x66, 0x66, 0x3E, 0x00,      // d
   0x00, 0x00, 0x3C, 0x66, 0x7E, 0x60, 0x3C, 0x00,      // e
   0x1C, 0x30, 0x30, 0x7C, 0x30, 0x30, 0x30, 0x00,      // f
   0x00, 0x00, 0x3E, 0x66, 0x66, 0x3E, 0x06, 0x3C,      // g
   0x60, 0x60, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x00,      // h
   0x18, 0x00, 0x38, 0x18, 0x18, 0x18, 0x3C, 0x00,      // i
   0x18, 0x00, 0x38, 0x18, 0x18, 0x18, 0x18, 0x70,      // j
   0x60, 0x60, 0x66, 0x6C, 0x78, 0x6C, 0x66, 0x00,      // k
   0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00,      // l
   0x00, 0x00, 0x36, 0x7F, 0x6B, 0x6B, 0x63, 0x00,      // m
   0x00, 0x00, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x00,      // n
   0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x00,      // o
   0x00, 0x00, 0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60,      // p
   0x00, 0x00, 0x3E, 0x66, 0x66, 0x3E, 0x06, 0x07,      // q
   0x00, 0x00, 0x6C, 0x76, 0x60, 0x60, 0x60, 0x00,      // r
   0x00, 0x00, 0x3E, 0x60, 0x3C, 0x06, 0x7C, 0x00,      // s
   0x30, 0x30, 0x7C, 0x30, 0x30, 0x30, 0x1C, 0x00,      // t
   0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3E, 0x00,      // u
   0x00, 0x00, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00,      // v
   0x00, 0x00, 0x63, 0x6B, 0x6B, 0x7F, 0x36, 0x00,      // w
   0x00, 0x00, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0x00,      // x
   0x00, 0x00, 0x66, 0x66, 0x66, 0x3E, 0x06, 0x3C,      // y
   0x00, 0x00, 0x7E, 0x0C, 0x18, 0x30, 0x7E, 0x00,      // z
   0x0C, 0x18, 0x18, 0x70, 0x18, 0x18, 0x0C, 0x00,      // {
   0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00,      // |
   0x30, 0x18, 0x18, 0x0E, 0x18, 0x18, 0x30, 0x00,      // }
   0x31, 0x6B, 0x46, 0x00, 0x00, 0x00, 0x00, 0x00,      // ~
   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,      //
};

static const char smallc[] = " 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-+&()/[];%";
static uint8_t const small[] = {
   0x00, 0x00, 0x00,            //
   0x1F, 0x11, 0x1F,            //0
   0x11, 0x1F, 0x10,            //1
   0x1D, 0x15, 0x17,            //2
   0x11, 0x15, 0x1F,            //3
   0x07, 0x04, 0x1F,            //4
   0x17, 0x15, 0x1D,            //5
   0x1F, 0x15, 0x1D,            //6
   0x01, 0x01, 0x1F,            //7
   0x1F, 0x15, 0x1F,            //8
   0x17, 0x15, 0x1F,            //9
   0x1E, 0x05, 0x1E,            //A
   0x1F, 0x15, 0x0A,            //B
   0x0E, 0x11, 0x11,            //C
   0x1F, 0x11, 0x0E,            //D
   0x1F, 0x15, 0x11,            //E
   0x1F, 0x05, 0x01,            //F
   0x0E, 0x11, 0x19,            //G
   0x1F, 0x04, 0x1F,            //H
   0x11, 0x1F, 0x11,            //I
   0x11, 0x0F, 0x01,            //J
   0x1F, 0x04, 0x1B,            //K
   0x1F, 0x10, 0x10,            //L
   0x1F, 0x03, 0x1F,            //M
   0x1F, 0x01, 0x1F,            //N
   0x0E, 0x11, 0x0E,            //O
   0x1F, 0x05, 0x02,            //P
   0x0E, 0x19, 0x1E,            //Q
   0x1F, 0x05, 0x1A,            //R
   0x12, 0x15, 0x09,            //S
   0x01, 0x1F, 0x01,            //T
   0x1F, 0x10, 0x1F,            //U
   0x0F, 0x10, 0x0F,            //V
   0x1F, 0x18, 0x1F,            //W
   0x1B, 0x04, 0x1B,            //X
   0x03, 0x1C, 0x03,            //Y
   0x19, 0x15, 0x13,            //Z
   0x04, 0x04, 0x04,            //-
   0x04, 0x0E, 0x04,            //+
   0x04, 0x0E, 0x04,            //& (+)
   0x00, 0x0E, 0x11,            //(
   0x11, 0x0E, 0x00,            //)
   0x08, 0x04, 0x02,            ///
   0x00, 0x1F, 0x11,            //[
   0x11, 0x1F, 0x00,            //]
   0x10, 0x0A, 0x00,            //;
   0x09, 0x04, 0x12,            //%
};

Image *
ImageNew (int w, int h, int c)
{                               // create a new blank image
   Image *i;
   if (!w || !h)
      return 0;
   i = malloc (sizeof (*i));
   if (!i)
      return 0;
   memset (i, 0, sizeof (*i));
   i->W = w;
   i->L = w + 1;
   i->H = h;
   i->C = c;
   i->Image = malloc ((w + 1) * h);
   if (!i->Image)
   {
      free (i);
      return 0;
   }
   memset (i->Image, 0, (w + 1) * h);
   if (c)
   {
      i->Colour = malloc (sizeof (Colour) * c);
      if (!i->Colour)
      {
         free (i->Image);
         free (i);
         return 0;
      }
      memset (i->Colour, 0, sizeof (Colour) * c);
   }
   return i;
}

void
ImageFree (Image * i)
{                               // free an image
   if (i)
   {
      if (i->Image)
         free (i->Image);
      if (i->Colour)
         free (i->Colour);
      free (i);
   }
}

#define	MAXLZW	4096
typedef short LZW[256];
typedef LZW LZWTree[MAXLZW];
typedef struct strPrivate
{
   int cols;                    // number of colours, power of 2
   uint8_t colbits;             // number of bits for colours
   FILE *f;                      // file handle
   int lzwnext;                 // next code
   int lzwlast;                 // last code in current bit size
   int lzwbits;                 // current bit size
   LZWTree lzw;                 // encode tree
   uint8_t block[256];          // block so far, with count at start
   int blockv;                  // pending value
   int blockb;                  // bits used in pending value
   short lzwcode;               // which code we are on now
}
Private;

static void
LZWFlush (Private * p)
{                               // flush this block
   fwrite (p->block, *p->block + 1,1,p->f);
   *p->block = 0;
}

static void
LZWOut (Private * p, short v)
{                               // output a value
   p->blockv |= (v << p->blockb);
   p->blockb += p->lzwbits;
   while (p->blockb >= 8)
   {
      p->block[++*p->block] = p->blockv;        // last partial byte
      p->blockv >>= 8;
      p->blockb -= 8;
      if (*p->block == 255)
         LZWFlush (p);
   }
}

static void
LZWClear (Private * p)
{
   int c;
   p->lzwbits = p->colbits + 1;
   p->lzwnext = p->cols + 2;
   p->lzwlast = (1 << p->lzwbits) - 1;
   p->lzwcode = p->cols;        // starting point
   for (c = 0; c < p->cols; c++)
   {
      p->lzw[p->cols][c] = c;   // links to literal entries
      memset (&p->lzw[c], -1, p->cols * 2);     // links from literals, dead ends initially
   }
}

static void
ImageStart (Private * p)
{
   fputc(p->colbits,p->f);
   *p->block = 0;
   p->blockb = 0;
   p->blockv = 0;
   LZWClear (p);
   LZWOut (p, p->cols);         // clear code
}

static void
ImageEnd (Private * p)
{
   LZWOut (p, p->lzwcode);      // last prefix
   LZWOut (p, p->cols + 1);     // end code
   if (p->blockb)
      p->block[++*p->block] = p->blockv;        // last partial byte
   LZWFlush (p);
}

static void
ImageOut (Private * p, uint8_t c)
{
   short next = p->lzw[p->lzwcode][c];
   if (next == -1)
   {                            // dead end
      LZWOut (p, p->lzwcode);   // prefix
#ifdef CLEAR
      if (p->lzwnext + 1 == MAXLZW)
      {
         LZWOut (p, p->cols);   // clear code
         LZWClear (p);
      } else
#endif
      if (p->lzwnext < MAXLZW)
      {
         memset (p->lzw[p->lzwnext], -1, p->cols * 2);  // init dead ends
         p->lzw[p->lzwcode][c] = p->lzwnext;
         if (p->lzwnext > p->lzwlast)
         {                      // bigger code
            p->lzwbits++;
            p->lzwlast = (1 << p->lzwbits) - 1;
         }
         p->lzwnext++;
      }
      p->lzwcode = c;
   } else
      p->lzwcode = next;        // not a dead end
}

// write GIF image
void
ImageWriteGif (Image * i, FILE *f, int back, int trans, char *comment)
{
   struct strPrivate p;
   p.f = f;
   p.colbits = 2;               // Count colours, min 4
   p.cols = 4;
   while (p.cols < i->C)
   {
      p.cols *= 2;
      p.colbits++;
   }
   {                            // headers
      uint8_t buf[1500];
      int n = 0;
      strcpy ((char *) buf, "GIF87a");
#ifndef INTERLACE
      if (comment || trans >= 0)
#endif
         buf[4] = '9';          // needs gif89 format
      n = 6;
      buf[n++] = (i->W & 255);
      buf[n++] = (i->W >> 8);
      buf[n++] = (i->H & 255);
      buf[n++] = (i->H >> 8);
      buf[n++] = (i->Colour ? 0x80 : 0) + 0x70 + (p.colbits - 1);
      buf[n++] = back;          // background
      buf[n++] = 0;             // aspect
      if (i->Colour)
      {
         int c;
         for (c = 0; c < p.cols; c++)
         {
            if (c < i->C)
            {
               buf[n++] = (i->Colour[c] >> 16 & 255);
               buf[n++] = (i->Colour[c] >> 8 & 255);
               buf[n++] = (i->Colour[c] & 255);
            } else
            {                   // extra, unused, colour
               buf[n++] = 0;
               buf[n++] = 0;
               buf[n++] = 0;
            }
         }
      }
      // comment
      if (comment && strlen (comment) < 256)
      {                         // comment
         buf[n++] = 0x21;       //extension
         buf[n++] = 0xFE;       //comment
         buf[n++] = strlen (comment);
         strcpy ((char *) buf + n, comment);
         n += buf[n - 1];
         buf[n++] = 0;          // end of block
      }
      if (trans >= 0)
      {                         // transparrent
         buf[n++] = 0x21;       // extension
         buf[n++] = 0xF9;       // graphic control
         buf[n++] = 4;          // len
         buf[n++] = 1;          // transparrent
         buf[n++] = 0;          // delay
         buf[n++] = 0;
         buf[n++] = trans;
         buf[n++] = 0;          // terminator
      }
      // image
      buf[n++] = 0x2C;
      buf[n++] = 0;             // offset X
      buf[n++] = 0;
      buf[n++] = 0;             // offset Y
      buf[n++] = 0;
      buf[n++] = (i->W & 255);
      buf[n++] = (i->W >> 8);
      buf[n++] = (i->H & 255);
      buf[n++] = (i->H >> 8);
#ifdef INTERLACE
      buf[n++] = 0x40;          // interlaced, no local colour table
#else
      buf[n++] = 0x00;          // non interlaced, no local colour table
#endif
      fwrite (buf, n,1,f);
   }
   // image data
   {
      uint8_t *b;
      int x,
        y;
      ImageStart (&p);
#ifdef INTERLACE
      for (y = 0; y < i->H; y += 8)
         for (b = &ImagePixel (i, 0, y), x = 0; x < i->W; x++)
            ImageOut (&p, *b++);
      for (y = 4; y < i->H; y += 8)
         for (b = &ImagePixel (i, 0, y), x = 0; x < i->W; x++)
            ImageOut (&p, *b++);
      for (y = 2; y < i->H; y += 4)
         for (b = &ImagePixel (i, 0, y), x = 0; x < i->W; x++)
            ImageOut (&p, *b++);
      for (y = 1; y < i->H; y += 2)
         for (b = &ImagePixel (i, 0, y), x = 0; x < i->W; x++)
            ImageOut (&p, *b++);
#else
      for (y = 0; y < i->H; y++)
         for (b = &ImagePixel (i, 0, y), x = 0; x < i->W; x++)
            ImageOut (&p, *b++);
#endif
      ImageEnd (&p);
   }
   fputc (0, f);         // end of image data
   fputc ('\x3B', f);       // trailer
}

void
ImageText (Image * i, int x, int y, int col, char *text)
{                               // writes 8x8 text
   if (i && text)
      while (*text)
      {
         if (*text >= ' ' && *text)
         {
            int r;
            unsigned const char *b = bbc + (*text - ' ') * 8;
            for (r = 0; r < 8; r++)
            {
               uint8_t v = *b++;
               uint8_t *p = &ImagePixel (i, x, y + r);
               uint8_t m;
               for (m = 0x80; m; m >>= 1, p++)
                  if (v & m)
                     *p = col;
            }
         }
         x += 8;
         text++;
      }
}

void
ImageSmall (Image * i, int x, int y, int col, char *text)
{                               // writes 4x6 digits
   if (i && text)
      while (*text)
      {
         char *p = strchr (smallc, toupper (*text));
         if (p)
         {
            int r;
            char m = 1;
            unsigned const char *b = small + (p - smallc) * 3;
            for (r = 0; r < 5; r++)
            {
               int c;
               for (c = 0; c < 3; c++)
                  if (b[c] & m)
                     ImagePixel (i, x + c, y + r) = col;
               m <<= 1;
            }
            x += 4;
         } else if (*text == '.')
         {
            ImagePixel (i, x, y + 4) = col;
            x += 2;
         } else if (*text == ':')
         {
            ImagePixel (i, x, y + 1) = col;
            ImagePixel (i, x, y + 3) = col;
            x += 2;
         }
         text++;
      }
}

 // Default pixel set op for text/small
void
ImageTextPixel (Image * i, int X, int Y, int x, int y, int c)
{
   ImagePixel (i, X + x, Y + y) = c;
}

void
ImageText5x9F (Image * i, int X, int Y, int col, char *text, ImagePixelFunc * f)
{                               // writes 5x9 text
   int x = 0,
      y = 0;
   if (i && text)
      while (*text)
      {
         if (*text >= ' ')
         {
            int r;
            unsigned const char *b = teletext + (*text - ' ') * 9;
            for (r = 0; r < 9; r++)
            {
               unsigned char c = 0;
               unsigned char v = *b++;
               unsigned char m;
               for (m = 0x80; m; m >>= 1, c++)
                  if (v & m)
                     f (i, X, Y, x + c, y + r, col);
            }
         }
         x += 6;
         text++;
      }
}

void
ImageText8x8F (Image * i, int X, int Y, int col, char *text, ImagePixelFunc * f)
{                               // writes 8x8 text
   int x = 0,
      y = 0;
   if (i && text)
      while (*text)
      {
         if (*text >= ' ')
         {
            int r;
            unsigned const char *b = bbc + (*text - ' ') * 8;
            for (r = 0; r < 8; r++)
            {
               unsigned char c = 0;
               unsigned char v = *b++;
               unsigned char m;
               for (m = 0x80; m; m >>= 1, c++)
                  if (v & m)
                     f (i, X, Y, x + c, y + r, col);
            }
         }
         x += 8;
         text++;
      }
}

void
ImageText3x5F (Image * i, int X, int Y, int col, char *text, ImagePixelFunc * f)
{                               // writes 3x5 digits/text
   int x = 0,
      y = 0;
   if (i && text)
      while (*text)
      {
         char *p = (char *) strchr (smallc, toupper (*text));
         if (p)
         {
            int r;
            char m = 1;
            unsigned const char *b = small + (p - smallc) * 3;
            for (r = 0; r < 5; r++)
            {
               int c;
               for (c = 0; c < 3; c++)
                  if (b[c] & m)
                     f (i, X, Y, x + c, y + r, col);
               m <<= 1;
            }
            x += 4;
         } else if (*text == '.')
         {
            f (i, X, Y, x, y + 4, col);
            x += 2;
         } else if (*text == ':')
         {
            f (i, X, Y, x, y + 1, col);
            f (i, X, Y, x, y + 3, col);
            x += 2;
         }
         text++;
      }
}

void
ImageRect (Image * i, int x, int y, int w, int h, int c)
{                               // fill a box
   if (!i)
      return;
   // range checks
   if (x < 0)
   {
      w += x;
      x = 0;
   }
   if (y < 0)
   {
      h += y;
      y = 0;
   }
   if (x + w > i->W)
      w = i->W - x;
   if (y + h > i->H)
      h = i->H - y;
   // sanity check
   if (w < 1 || h < 1)
      return;
   while (h--)
   {
      uint8_t *p = &ImagePixel (i, x, y);
      int n = w;
      while (n--)
         *p++ = c;
      y++;
   }
}

// PNG code

      /* Table of CRCs of all 8-bit messages. */
static uint32_t crc_table[256];

      /* Make the table for a fast CRC. */
void
make_crc_table (void)
{
   uint32_t c;
   int n,
     k;
   for (n = 0; n < 256; n++)
   {
      c = (uint32_t) n;
      for (k = 0; k < 8; k++)
      {
         if (c & 1)
            c = 0xedb88320L ^ (c >> 1);
         else
            c = c >> 1;
      }
      crc_table[n] = c;
   }
}

      /* Update a running CRC with the bytes buf[0..len-1]--the CRC
         should be initialized to all 1's, and the transmitted value
         is the 1's complement of the final running CRC (see the
         crc() routine below)). */

uint32_t
update_crc (uint32_t crc, uint8_t * buf, int len)
{
   uint32_t c = crc;
   int n;

   for (n = 0; n < len; n++)
      c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);

   return c;
}

      /* Return the CRC of the bytes buf[0..len-1]. */
uint32_t
crc (uint8_t * buf, int len)
{
   return update_crc (0xffffffffL, buf, len) ^ 0xffffffffL;
}

uint32_t
writecrc (FILE *f, uint8_t * ptr, int len, uint32_t c)
{
   fwrite (ptr, len,1,f);
   while (len--)
      c = crc_table[(c ^ *ptr++) & 0xff] ^ (c >> 8);
   return c;
}

void
writechunk (FILE *f, char *typ, void *ptr, int len)
{
   uint32_t v = htonl (len),
      crc;
   fwrite ( &v, 4,1,f);
   crc = writecrc (f, (uint8_t *) typ, 4, 0xFFFFFFFF);
   if (len)
      crc = writecrc (f, ptr, len, crc);
   v = htonl (~crc);
   fwrite (&v, 4,1,f);
}

uint32_t
adlersum (uint8_t * p, int l, uint32_t adler)
{
   uint32_t s1 = (adler & 65535),
      s2 = (adler >> 16);
   while (l--)
   {
      s1 += *p++;
      s2 += s1;
   }
   s1 %= 65521;                 // can be delayed due to sensible "l" values...
   s2 %= 65521;
   return (s2 << 16) + s1;
}

// write PNG image
void
ImageWritePNGz (Image * i, FILE *f, int back, int trans, unsigned int dpm, char *comment, int z)
{
   make_crc_table ();
   fwrite ("\211PNG\r\n\032\n", 8,1,f);  // PNG header
   {                            // IHDR
      struct
      {
         uint32_t width;
         uint32_t height;
         uint8_t depth;
         uint8_t colour;
         uint8_t compress;
         uint8_t filter;
         uint8_t interlace;
      }
      ihdr =
      {
      0, 0, 8, 3, 0, 0};
      ihdr.width = htonl (i->W);
      ihdr.height = htonl (i->H);
      writechunk (f, "IHDR", &ihdr, 13);
   }
   {                            // PLTE
      uint32_t v = htonl (i->C * 3),
         crc,
         n;
      fwrite ( &v, 4,1,f);
      crc = writecrc (f, (uint8_t *) "PLTE", 4, 0xFFFFFFFF);
      for (n = 0; n < i->C; n++)
      {
         v = htonl (i->Colour[n] << 8);
         crc = writecrc (f, (void *) &v, 3, crc);
      }
      v = htonl (~crc);
      fwrite (&v, 4,1,f);
   }
   if (back >= 0)
   {                            // bKGD
      uint8_t b = back;
      writechunk (f, "bKGD", &b, 1);
   }
   if (dpm >= 0)
   {                            // Pixels per metre
      struct
      {
         uint32_t x;            // pixels per metre
         uint32_t y;            // pixels per metre
         uint8_t type;          // 1 means metre , 0 means unspecified (defines ratio only)
      }
      phys =
      {
      htonl (dpm), htonl (dpm), 1};
      writechunk (f, "pHYs", &phys, 9);
   }
   if (*comment)
   {                            // tEXt
      char c[] = "Comment";
      uint32_t v = htonl (strlen (c) + strlen (comment) + 1),
         crc;
      fwrite ( &v, 4,1,f);
      crc = writecrc (f, (uint8_t *) "tEXt", 4, 0xFFFFFFFF);
      crc = writecrc (f, (uint8_t *) c, strlen (c) + 1, crc);
      crc = writecrc (f, (uint8_t *) comment, strlen (comment), crc);
      v = htonl (~crc);
      fwrite (&v, 4,1,f);
   }
   {                            // tRNS
      uint8_t alpha[256];
      int n;
      for (n = 0; n < i->C; n++)
         alpha[n] = 255 - (i->Colour[n] >> 24); // 4th palette byte treated as 0=opaque, 255-transparrent
      if (trans >= 0 && trans < i->C)
         alpha[trans] = 0;      // manual set of specific transparrent colour
      writechunk (f, "tRNS", alpha, i->C);
   }
#ifdef USEZLIB
   if (z)
   {                            // IDAT
      uint8_t *temp;
      unsigned long n;
      for (n = 0; n < i->H; n++)
         i->Image[n * i->L] = 0;        // filter 0
      n = i->H * i->L * 1001 / 1000 + 12;
      temp = malloc (n);
      if (compress2 (temp, &n, i->Image, i->L * i->H, z) != Z_OK)
         fprintf (stderr, "Deflate error\n");
      else
         writechunk (f, "IDAT", temp, n);
      free (temp);
   } else
#endif
   {                            // IDAT
      uint32_t v = htonl (i->H * (i->L + 5) + 6),
         crc,
         adler = 1,
         n;
      uint8_t *p = i->Image;
      fwrite (&v, 4,1,f);
      crc = writecrc (f, (unsigned char *) "IDAT", 4, 0xFFFFFFFF);
      crc = writecrc (f, (unsigned char *) "\170\001", 2, crc);        // zlib header for deflate
      n = i->H;
      while (n--)
      {
         uint8_t h[5];
         h[0] = (n ? 0 : 1);    // last chunk in deflate, un compressed
         h[1] = (i->L & 255);   // Len, LSB first as per deflate spec
         h[2] = (i->L / 256);
         h[3] = ~(i->L & 255);  // Inverse of Len
         h[4] = ~(i->L / 256);
         *p = 0;                // filter 0 (NONE)
         crc = writecrc (f, h, 5, crc);
         crc = writecrc (f, p, i->L, crc);
         adler = adlersum (p, i->L, adler);
         p += i->L;
      }
      v = htonl (adler);
      crc = writecrc (f, (void *) &v, 4, crc);
      v = htonl (~crc);
      fwrite ( &v, 4,1,f);
   }
   writechunk (f, "IEND", 0, 0);       // IEND
}

void
PathInit (Path * p)
{                               // Initialise a path
   if (!p)
      return;
   p->last = 0;
   p->skip = 0;
   p->noskip = 0;
   return;
}

static void
PathDelta (Path * p)
{
   if (p->skipx == p->lastx)

      snprintf (p->text, sizeof (p->text), "v%d", p->skipy - p->lasty);
   else if (p->skipy == p->lasty)

      snprintf (p->text, sizeof (p->text), "h%d", p->skipx - p->lastx);
   else
      snprintf (p->text, sizeof (p->text), "l%d,%d", p->skipx - p->lastx, p->skipy - p->lasty);
   p->lastx = p->skipx;
   p->lasty = p->skipy;
}

static int
PathLine (Path * p, int x, int y)
{
   if (p->noskip)
      return 0;                 // Don't skip any, e.g. marker-mid is used
   if ((x == p->skipx && x == p->lastx && y > p->skipy && p->skipy > p->lasty) ||       //
       (x == p->skipx && x == p->lastx && y < p->skipy && p->skipy < p->lasty) ||       //
       (y == p->skipy && y == p->lasty && x > p->skipx && p->skipx > p->lastx) ||       //
       (y == p->skipy && y == p->lasty && x < p->skipx && p->skipx < p->lastx))
   {                            // Continuing along a line
      p->skipx = x;
      p->skipy = y;
      return 1;
   }
   return 0;
}

char *
PathPlot (Path * p, int x, int y)
{                               // add a point, returns string to output or NULL
   if (!p)
      return NULL;
   if (p->skip && p->skipx == x && p->skipy == y)
      return NULL;              // Duh
   if (!p->last && !p->skip)
   {                            // Just record first point
      p->initx = x;
      p->inity = y;
      p->skipx = x;
      p->skipy = y;
      p->skip = 1;
      return NULL;
   }
   if (!p->last)
   {                            // We have started, Set M for original point
      p->lastx = p->skipx;
      p->lasty = p->skipy;
      p->last = 1;
      p->skipx = x;
      p->skipy = y;
      snprintf (p->text, sizeof (p->text), "M%d,%d", p->lastx, p->lasty);
      return p->text;
   }
   if (PathLine (p, x, y))
      return NULL;
   PathDelta (p);
   p->skipx = x;
   p->skipy = y;
   return p->text;
}

char *
PathDone (Path * p)
{                               // finish path, returns string to output or NULL
   if (!p)
      return NULL;
   if (!p->last)
      return NULL;              // None, or one point, do nothing
   PathLine (p, p->initx, p->inity);
   if (p->skipx != p->initx || p->skipy != p->inity)
   {
      PathDelta (p);
      p->last = 0;
      p->skip = 0;
      return p->text;
   }
   p->last = 0;
   p->skip = 0;
   return NULL;
}

void
ImageSVGPath (Image * i,
#ifdef	FIREBRICK
              stream_h o,
#else
              FILE * o,
#endif
              int c
#ifdef FIREBRICK
              , heap_h heap
#endif
   )
{
   unsigned char *map = malloc (i->W * i->H);
   if (!map)
      return;
   memset (map, 0, i->W * i->H);
   int col (int x, int y)
   {                            // Safe colour check of pixel
      if (x < 0 || x >= i->W || y < 0 || y >= i->H)
         return -1;
      return ImagePixel (i, x, y);

   }
   int y;
   for (y = 0; y < i->H; y++)
   {
      int x;
      for (x = 0; x < i->W; x++)
      {
         if (col (x, y) != c)
            continue;           // Not the colour I am drawing
         int d = 0;             // Direction
         const int dx[] = { 0, 1, 0, -1, 0, 1, 0 };     // Direction
         const int dy[] = { -1, 0, 1, 0, -1, 0, 1 };
         const int cx[] = { 1, 1, 0, 0, 1, 1, 0 };      // Corner
         const int cy[] = { 0, 1, 1, 0, 0, 1, 1 };
         for (d = 0; d < 4 && ((map[x + y * i->W] & (1 << d)) || col (x + dx[d], y + dy[d]) == c); d += 2);     // Can only be 0 or 2
         if (d == 4)
            continue;           // Middle of a block
         Path p;
         PathInit (&p);
         char *t;
         if (!d)
         {                      // exterior, so start on corner to help optimisation
            t = PathPlot (&p, x + cx[d + 3], y + cy[d + 3]);
            if (t)
               fprintf (o, "%s", t);
         }
         while (!(map[x + y * i->W] & (1 << d)))
         {
            t = PathPlot (&p, x + cx[d], y + cy[d]);
            if (t)
               fprintf (o, "%s", t);
            map[x + y * i->W] |= (1 << d);
            if (col (x + dx[d + 1] + dx[d], y + dy[d + 1] + dy[d]) == c)
            {                   // Can turn left
               x += dx[d + 1] + dx[d];
               y += dy[d + 1] + dy[d];
               d = ((d + 3) & 3);
            } else if (col (x + dx[d + 1], y + dy[d + 1]) == c)
            {                   // Can move right
               x += dx[d + 1];
               y += dy[d + 1];
            } else
            {                   // Turn right
               d = ((d + 1) & 3);
            }
         }
         t = PathDone (&p);
         if (t)
            fprintf (o, "%s", t);
      }
   }
   free (map);
}

void
ImageWriteSVG (Image * i,
#ifdef	FIREBRICK
               stream_h o,
#else
               FILE *o,
#endif
               int back, int trans, char *comment, double scale
#ifdef FIREBRICK
               , heap_h heap
#endif
   )
{                               // Write image as SVG (path around pixels of each colour)
   fprintf (o, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
   if (comment && !strstr (comment, "-->"))
      fprintf (o, "<!-- %s -->\n", comment);
   fprintf (o, "<svg xmlns:svg=\"http://www.w3.org/2000/svg\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.0\" ");
   if (scale > 0)
      fprintf (o, "width=\"%.2fmm\" height=\"%.2fmm\" viewBox=\"0 0 %d %d\"", (double) i->W * scale, (double) i->H * scale, i->W,
               i->H);
   else
      fprintf (o, "width=\"%d\" height=\"%d\"", i->W, i->H);
   fprintf (o, ">");
   void addcolour (int c)
   {
      unsigned char r = (i->Colour[c] >> 16),
         g = (i->Colour[c] >> 8),
         b = (i->Colour[c]);
      if (!(r % 17) && !(g % 17) && !(b % 17))
         fprintf (o, "#%X%X%X", r / 17, g / 17, b / 17);
      else
         fprintf (o, "#%02X%02X%02X", r, g, b);
   }
   // Background colour 0, unless that is trans
   if (back >= 0 && back != trans)
   {
      fprintf (o, "<rect width=\"%d\" height=\"%d\" fill=\"", i->W, i->H);
      addcolour (0);
      fprintf (o, "\"/>");
   }
   // Scan colours
   int c;
   for (c = 0; c < i->C; c++)
      if (c != back && c != trans)
      {
         fprintf (o, "<path fill=\"");
         addcolour (c);
         fprintf (o, "\" d=\"");
         ImageSVGPath (i, o, c
#ifdef	FIREBRICK
                       , h
#endif
            );
         fprintf (o, "\"/>");
      }
   fprintf (o, "</svg>");
   fflush (o);
}
