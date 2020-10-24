#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MENU_CONFIG_FILE_NAME "menu.cfg"
#define FIELD_CONFIG_FILE_NAME "fields.cfg"
#define UPDATABLE_FIELD_FILE_NAME "updatableFields.cfg"
#define DATA_FILE_NAME "data.dat"
#define LENGTH_OF_field_name 25
#define LENGTH_OF_EXISTING_DATA 20
#define LENGTH_OF_FIELD_VALUE 20

void display_menu();
int add_record();
int print_records();
int update_record();
int deactivate_record();

void initialize_global_variables();
FILE* open_file(char*, char*);
int get_count_of_fields();
void copy_fields_in_variables();
int get_count_of_updatable_fields();
void copy_updatable_fields();
void print_record_not_found();

FILE* fp_fields;
FILE* fp_updatable_fields;
char field_name[LENGTH_OF_field_name];
char field_value[LENGTH_OF_FIELD_VALUE];
char user_input_data[LENGTH_OF_EXISTING_DATA];
int count_of_fields, row_index, count_of_updatable_fields;
char user_given_id_to_find[LENGTH_OF_FIELD_VALUE];
char record_status;
char** field_names;
int* updatable_fields;

int main()
{
	display_menu();
	return 0;
}

void display_menu()
{
	initialize_global_variables();
	char menu[250];
	FILE* fp_menu = open_file(MENU_CONFIG_FILE_NAME, "r");
	fread(menu, sizeof(menu), 1, fp_menu);
	fclose(fp_menu);
	int user_option, is_record_saved, is_record_updated, count_of_records, is_record_deleted;
	menu[strlen(menu) - 1] = '\0';
	while(1)
	{
		printf("%s\n", menu);
		printf("Enter your choice: ");
		scanf("%d", &user_option);
		switch(user_option)
		{
			case 1:
				is_record_saved = add_record();
				is_record_saved > 0 ? puts("The details you entered are saved successfully.\n") : puts("Error in while saving the record.\n");
				break;
			case 2:
				count_of_records = print_records();
				printf("Number of record(s): %d", count_of_records);
				break;
			case 3:
				is_record_updated = update_record();
				if(is_record_updated == 0)
					print_record_not_found();
				break;
			case 4:
				is_record_deleted = deactivate_record();
				if(is_record_deleted == 0)
					print_record_not_found();
				break;
			case 5:
				printf("--------------------------\n");
				printf("Entered exit as choice.");
				exit(0);
			default:
				printf("Invalid choice or entered inappropriate data.");
		}
		printf("\n-----------------------\n");
	}
}


FILE* open_file(char* fileName, char* mode)
{
	FILE* fp = fopen(fileName, mode);
	if(fp == NULL)
	{
		printf("File not found or error in opening the file.\n");
		exit(1);
	}
	return fp;
}

void initialize_global_variables()
{
	fp_fields = open_file(FIELD_CONFIG_FILE_NAME, "r");
	fp_updatable_fields = open_file(UPDATABLE_FIELD_FILE_NAME, "r");
	count_of_fields = get_count_of_fields();
	copy_fields_in_variables();
	count_of_updatable_fields = get_count_of_updatable_fields();
	copy_updatable_fields();
	fclose(fp_fields);
	fclose(fp_updatable_fields);
}

int get_count_of_fields()
{
	int fields_counter = 0;
	while(fgets(field_name, sizeof(field_name), fp_fields))
	{
		fields_counter++;
	}
	return fields_counter;
}

void copy_fields_in_variables()
{
	rewind(fp_fields);
	field_names = malloc(count_of_fields * sizeof(char*));
	for(row_index = 0; row_index < count_of_fields; row_index++)
	{
		field_names[row_index] = malloc(LENGTH_OF_field_name);
		fgets(field_name, sizeof(field_name), fp_fields);
		field_name[strlen(field_name) - 1] = '\0';
		strcpy(field_names[row_index], field_name);
	}
}
int get_count_of_updatable_fields()
{
	int updatable_fields_counter = 0;
	char updatableFieldPosition[3];
	while(fgets(updatableFieldPosition, sizeof(updatableFieldPosition), fp_updatable_fields) != NULL)
	{
		updatable_fields_counter++;
	}
	return updatable_fields_counter;
}

void copy_updatable_fields()
{
	rewind(fp_updatable_fields);
	row_index = 0;
	char updatableFieldPosition[3];
	updatable_fields = malloc(count_of_updatable_fields * sizeof(int));
	while(fgets(updatableFieldPosition, sizeof(updatableFieldPosition), fp_updatable_fields) != NULL)
	{
		updatableFieldPosition[strlen(updatableFieldPosition) - 1] = '\0';
		updatable_fields[row_index] = atoi(updatableFieldPosition) - 1;
		row_index++;
	}
}

int add_record()
{
	FILE* fp_data = open_file(DATA_FILE_NAME, "a");
	int is_record_saved = 0;
	record_status = 'a';
	fwrite(&record_status, sizeof(record_status), 1, fp_data);
	for(row_index = 0; row_index < count_of_fields; row_index++)
	{
		fflush(stdin);
		printf("Enter %s: ", field_names[row_index]);
		scanf("%s", field_value);
		is_record_saved = fwrite(field_value, sizeof(field_value), 1, fp_data);
	}
	fclose(fp_data);
	return is_record_saved;
}

int print_records()
{
	FILE* fp_data = open_file(DATA_FILE_NAME, "r");
	int count_of_records = 0;
	char field_value[LENGTH_OF_FIELD_VALUE];
	while(fread(&record_status, sizeof(record_status), 1, fp_data))
	{
		if(record_status == 'a')
		{
			printf("-----------------------------\n");
			for(row_index = 0; row_index < count_of_fields; row_index++)
			{
				fread(field_value, sizeof(field_value), 1, fp_data);
				printf("%s: %s\n", field_names[row_index], field_value);
			}
			printf("-----------------------------\n");
			count_of_records++;
		}
		else
		{
			fseek(fp_data, count_of_fields * LENGTH_OF_FIELD_VALUE, SEEK_CUR);
		}
	}
	fclose(fp_data);
	return count_of_records;
}

int update_record()
{
	FILE* fp_data = open_file(DATA_FILE_NAME, "r+");
	int is_record_updated = 0;
	row_index = 0;
	printf("Enter %s to update Information: ", field_names[row_index]);
	scanf("%s", user_given_id_to_find);
	while(fread(&record_status, sizeof(record_status), 1, fp_data))
	{
		fread(user_input_data, sizeof(user_input_data), 1, fp_data);
		if((record_status == 'a') && (strcmp(user_input_data, user_given_id_to_find) == 0))
		{
			is_record_updated = 1;
			int user_choice;
			for(row_index = 0; row_index < count_of_updatable_fields; row_index++)
			{
				printf("%d. Update %s\n", row_index + 1, field_names[updatable_fields[row_index]]);
			}
			printf("Enter your choice: ");
			scanf("%d", &user_choice);
			if(user_choice <= count_of_updatable_fields)
			{
				fseek(fp_data,  (updatable_fields[user_choice - 1] - 1) * sizeof(user_input_data), SEEK_CUR);
				printf("Enter new %s: ", field_names[updatable_fields[user_choice - 1]]);
				scanf("%s", field_value);
				fwrite(field_value, sizeof(field_value), 1, fp_data);
				printf("%s updated successfully", field_names[updatable_fields[user_choice - 1]]);
				break;
			}
			puts("Invalid update choice");
			break;
		}
		else
		{
			fseek(fp_data, (count_of_fields - 1) * LENGTH_OF_FIELD_VALUE, SEEK_CUR);
		}
	}
	fclose(fp_data);
	return is_record_updated;
}

int deactivate_record()
{
	FILE* fp_data = open_file(DATA_FILE_NAME, "r+");
	int is_record_deleted = 0;
	row_index = 0;
	printf("Enter %s to delete: ", field_names[row_index]);
	scanf("%s", user_given_id_to_find);
	while(fread(&record_status, sizeof(record_status), 1, fp_data))
	{
		fread(user_input_data, sizeof(user_input_data), 1, fp_data);
		if((record_status == 'a') && (strcmp(user_input_data, user_given_id_to_find) == 0))
		{
			fseek(fp_data, (sizeof(user_input_data) + 1) * (-1), SEEK_CUR);
			record_status = 'i';
			is_record_deleted = fwrite(&record_status, sizeof(record_status), 1, fp_data);
			printf("%s and details are deleted successfully.", field_names[row_index]);
			break;
		}
		else
		{
			fseek(fp_data, (count_of_fields - 1) * sizeof(user_input_data), SEEK_CUR);
		}
	}
	fclose(fp_data);
	return is_record_deleted;
}

void print_record_not_found()
{
	row_index = 0;
	printf("%s and details are not found\n", field_names[row_index]);
}