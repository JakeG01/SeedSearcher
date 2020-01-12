﻿#pragma once
#include "Type.h"
#include "XoroshiroState.h"

inline _u64 GetSignature(_u64 value)
{
	unsigned int a = (unsigned int)(value ^ (value >> 32));
	a = a ^ (a >> 16);
	a = a ^ (a >> 8);
	a = a ^ (a >> 4);
	a = a ^ (a >> 2);
	return (a ^ (a >> 1)) & 1;
}

struct PokemonData
{
	int ivs[6];
	int ability;
	int nature;
	int characteristic;
	int fixedIV;
	bool isNoGender;
	bool isEnableDream;

	inline bool IsCharacterized(int index) // H A B "S" C D
	{
		switch (index)
		{
			case 0: return (ivs[0] == 31);
			case 1: return (ivs[1] == 31);
			case 2: return (ivs[2] == 31);
			case 3: return (ivs[5] == 31);
			case 4: return (ivs[3] == 31);
			case 5: return (ivs[4] == 31);
			default: return true;
		}
	}
};



inline bool TestPkmn(XoroshiroState xoroshiro, PokemonData pkmn) {
	while (xoroshiro.Next(0xFFFFFFFFu) == 0xFFFFFFFFu); // EC
	while (xoroshiro.Next(0xFFFFFFFFu) == 0xFFFFFFFFu); // OTID
	while (xoroshiro.Next(0xFFFFFFFFu) == 0xFFFFFFFFu); // PID

	int ivs[6] = { -1, -1, -1, -1, -1, -1 };
	int fixedCount = 0;
	do {
		int fixedIndex = 0;
		do {
			fixedIndex = xoroshiro.Next(7); // V箇所
		} while (fixedIndex >= 6);

		if (ivs[fixedIndex] == -1)
		{
			ivs[fixedIndex] = 31;
			++fixedCount;
		}
	} while (fixedCount < pkmn.fixedIV);

	// 個体値
	bool isPassed = true;
	for (int i = 0; i < 6; ++i)
	{
		if (ivs[i] == 31)
		{
			if (pkmn.ivs[i] != 31)
			{
				isPassed = false;
				break;
			}
		}
		else if (pkmn.ivs[i] != xoroshiro.Next(0x1F))
		{
			isPassed = false;
			break;
		}
	}
	if (!isPassed)
	{
		return false;
	}

	// 特性
	int ability = 0;
	if (pkmn.isEnableDream)
	{
		do {
			ability = xoroshiro.Next(3);
		} while (ability >= 3);
	}
	else
	{
		ability = xoroshiro.Next(1);
	}
	if ((pkmn.ability >= 0 && pkmn.ability != ability) || (pkmn.ability == -1 && ability >= 2))
	{
		return false;
	}

	// 性別値
	if (!pkmn.isNoGender)
	{
		int gender = 0;
		do {
			gender = xoroshiro.Next(0xFF);
		} while (gender >= 253);
	}

	// 性格
	int nature = 0;
	do {
		nature = xoroshiro.Next(0x1F);
	} while (nature >= 25);

	return nature == pkmn.nature;
}