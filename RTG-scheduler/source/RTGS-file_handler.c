/*
* RTGS-file_handler.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

#define FILE_MAX_KERNELS 60

/* The Function is for reading the GPU Compatilble Kernel Values */
int Get_kernel_information(Kernel_INFO* kernel, const char *File)
{
	char ch[FILE_MAX_KERNELS];
	char PN[10], TEXE[10], TD[10], TLS[10], KN[10];
	int i, j, k, flag, kn, Nkr = 0;

	FILE * fp;
	fp = fopen(File, "r");		// read mode
	if (fp == NULL) { printf("Error while opening the file.\n");	return RTGS_FAILURE; }

	while (fgets(ch, FILE_MAX_KERNELS, fp) != NULL)
	{
		KN[0] = 0;
		PN[0] = 0;
		TEXE[0] = 0;
		TD[0] = 0;
		TLS[0] = 0;

		for (k = 0; k < 10; k++)
		{
			KN[k] = '\0';
			PN[k] = '\0';
			TEXE[k] = '\0';
			TD[k] = '\0';
			TLS[k] = '\0';
		}

		i = 0;
		flag = 0;
		j = 0;

		while (ch[i] != '\0')
		{
			if (ch[i] == ' ')
			{
				flag = flag + 1;
				j = 0;
			}
			if (ch[i] != ' ' && flag == 0)
			{
				KN[j] = ch[i];
				j++;
			}
			if (ch[i] != ' ' && flag == 1)
			{
				PN[j] = ch[i];
				j++;
			}
			if (ch[i] != ' ' && flag == 2)
			{
				TEXE[j] = ch[i];
				j++;
			}
			if (ch[i] != ' ' && flag == 3)
			{
				TD[j] = ch[i];
				j++;
			}
			if (ch[i] != ' ' && flag == 4)
			{
				TLS[j] = ch[i];
				j++;
			}
			i++;
		}

		if (ch[i] == '\0')
		{
			kn = atoi(KN);
			kernel[kn].Pn = atoi(PN);
			kernel[kn].Texe = atoi(TEXE);
			kernel[kn].Td = atoi(TD);
			kernel[kn].Tls = atoi(TLS);
			Nkr++;
		}
	}
	fclose(fp);

	return (Nkr);
}


/* The Function is to read the time frames in which these Kernels are released */
int Get_kernel_release_times(const char *File)
{
	char ch[FILE_MAX_KERNELS];// file_name[25];
	int i = 0;

	FILE * fp;
	fp = fopen(File, "r");		// read mode
	if (fp == NULL) { printf("Error while opening the file.\n");	return RTGS_FAILURE; }

	while (fgets(ch, 2, fp) != NULL)
	{

		if (ch[0] == '0')
		{
			RT[i] = 0;
			i++;
		}
		else if (ch[0] == '1')
		{
			RT[i] = 1;
			i++;
		}
		else if (ch[0] == '2')
		{
			RT[i] = 2;
			i++;
		}

	}
	fclose(fp);

	return i;
}



