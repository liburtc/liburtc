/*
 * liburtc
 * Copyright (C) 2019 Chris Hiszpanski
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "string.h"

int main(int argc, char **argv) {
	{
		string_t *s = string_create_empty(3);
		string_append(s, "one");
		string_append(s, "two");
		string_append(s, "three");
		string_destroy(s);
	}

	return 0;
}
