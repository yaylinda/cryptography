#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define DWORD unsigned long long
#define EPSILON 0.00001f

/////////////////////////////
// Private Prototypes
bool open_files(const char *source_filename, const char *dest_filename, FILE **source, FILE **dest);
bool process_file(FILE *source, FILE *dest, const char *password);
DWORD find_source_size(FILE *source);
void crypt_buffer(const char *source_buffer, char *dest_buffer, int max_size, int process_size, const char *password);
char stream_value(const char *password, int index);

/////////////////////////////
// Public Functions
int main(int argc, char *argv[])
{
	int result = 20;

	if (argc != 4) 
	{
		printf("Usage: test source-filename dest-filename password.\n");
		result = 0;
	} 
	else
	{
		FILE *source;
		FILE *dest;
		bool open_files_test;
		open_files_test = open_files(argv[1], argv[2], &source, &dest);
		if (open_files_test) 
		{
			bool process_file_test;
			process_file_test = process_file(source, dest, argv[3]);

			if (process_file_test)
			{
				fclose(source);
				fclose(dest);
				result = 0;
			}
			else
			{
				result = 1;
			}
		}
		else
		{
			result = 1;
		}
	} 

	return result;
}

/////////////////////////////
// Private Functions
bool open_files(const char* source_filename, const char* dest_filename, FILE **source, FILE **dest)
{
	// read file
	bool result = false;

	*source = fopen(source_filename, "rb");
	if (*source != NULL) 
	{
		*dest = fopen(dest_filename, "wb");
		if (*dest != NULL)
		{
			result = true;
		}
		else
		{
			printf("Could not open destination file for writing.\n");
		}

	} 
	else
	{
		printf("Could not open source file for reading.\n");
	}

	return result;
}

bool process_file(FILE *source, FILE *dest, const char *password)
{
	bool result = true;

	float previous_percentile = 0.0f;
	float diff_percentile = 0.0f;
	const float increment = 0.05f;

	DWORD source_size;
	source_size = find_source_size(source);

	char source_buffer[2048];
	char dest_buffer[2048];

	DWORD amount_left;
	amount_left = source_size;

	printf("[");

	while (amount_left > 0)
	{
		int amount_read;
		int amount_written;
		amount_read = fread(source_buffer, sizeof(source_buffer[0]), sizeof(source_buffer), (FILE*)source);
		
		crypt_buffer(source_buffer, dest_buffer, sizeof(source_buffer), amount_read, password);

		amount_written = fwrite(dest_buffer, sizeof(dest_buffer[0]), amount_read, dest);
		amount_left -= amount_read;
		
		float current_percentile;

		// printf("%s %d\n", "amount_read", amount_read);
		current_percentile = (float)(source_size - amount_left) / source_size;
		// printf("%s %f\n", "current_percentile", current_percentile);
		diff_percentile += current_percentile - previous_percentile;
		// printf("%s %f\n", "diff_percentile", diff_percentile);
		
		bool flag = false;
		while ((diff_percentile + EPSILON) > increment)
		{
			printf("-");
			diff_percentile -= increment;
			flag = true;
		}

		if (flag) 
		{
			previous_percentile = current_percentile;
		}

		if (amount_read != amount_written)
		{
			result = false;
			printf("IO error. Amount read != amount written.\n");
			break;
		}

		if ((amount_read == 0) && (amount_left != 0))
		{
			result = false;
			break;
		}
	}

	printf("]\n");

	return result;
}

DWORD find_source_size(FILE *source)
{
	DWORD result;
	DWORD org_pos;

	org_pos = ftell(source);
	fseek(source, 0, SEEK_END);
	result = ftell(source);
	fseek(source, org_pos, SEEK_SET);

	return result;
}

void crypt_buffer(const char *source_buffer, char *dest_buffer, int max_size, int process_size, const char *password)
{
	int i;
	for (i=0; i<process_size; i++)
	{
		dest_buffer[i] = source_buffer[i] ^ stream_value(password, i);
	}
}

char stream_value(const char *password, int index)
{
	char result;

	int str_size;
	str_size = strlen(password);

	result = password[index%str_size];

	// FILE *s;
	// s = fopen("stream.txt", "a+t");
	// fprintf(s, "%c", result);
	// fclose(s);

	return result;
}







