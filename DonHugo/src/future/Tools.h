#pragma once

#include <stdint.h>

#include <string>

bool eqArray(const void* arr1, const void* arr2, uint32_t nBytes);

float convertBytes(uint32_t nBytes, const char** sizeName);

std::string extractNameFromFilepath(const std::string& filepath);

long normVolumeToDB(float volume);