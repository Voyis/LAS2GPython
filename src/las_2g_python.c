/**
 * @file las_2g_python_module.h
 * @author Ryan Wicks
 * @brief reader and write methods for the LAS files.
 * @version 0.1
 * @date 2020-03-07
 * 
 * @copyright 2G Robotics Inc., Copyright (c) 2020
 * 
 */

#include "las_2g_python.h"
#include <string.h>


const double header_scale = 0.000001;
const double point_scale = 1000000.;
const uint64_t diff_to_gps_epoch = (uint64_t)315964800 *(uint64_t)1000000;

int read_header(FILE * fid, LASHeader * header) {
    return fread((void *)header, sizeof(LASHeader), 1, fid);
}

int read_entry(FILE * fid, LASEntry * entries, size_t number_of_entries) {
    return fread((void *)entries, sizeof(LASEntry), number_of_entries, fid);
}

int write_header(FILE * fid, LASHeader * header) {
    return fwrite((void *)header, sizeof(LASHeader), 1, fid);
}

int write_entries(FILE * fid, LASEntry * entries, size_t number_of_entries) {
    return fwrite((void *)(entries), sizeof(LASEntry), number_of_entries, fid);
}

int write_las( const char * filename, LASFile las_files[], size_t number_of_file_entries) {
    FILE * fid;

    fid = fopen(filename, "wb");
    if (fid == NULL) {
        printf("Failed to open file %s\n", filename);
        return -1;
    }

    for (size_t i = 0; i < number_of_file_entries; ++i) {
        write_header (fid, las_files[i].header);
        int number_of_entries = las_files[i].header->number_of_point_records;
        write_entries(fid, las_files[i].entries, number_of_entries);
    }

    fclose(fid);

    return 0;
}

int read_las( const char * filename, LASFile *** las_files){
    FILE * fid;

    *las_files = NULL;

    LASFile * temp_files[1024];
    int temp_count = 0;
    int number_of_files = 0;
    size_t return_value;
    fid = fopen(filename, "rb");
    if (fid == NULL) {
        printf("Failed to open file %s\n", filename);
        return -1;
    }
    while (!feof(fid)){
        temp_files[temp_count] = (LASFile*)malloc(sizeof(LASFile));
        temp_files[temp_count]->header = (LASHeader * )malloc(sizeof(LASHeader));
        return_value = read_header(fid, temp_files[temp_count]->header);
        if (return_value != 1) { //fread returns the number of HEADER_SIZE read.
            free(temp_files[temp_count]->header);
            free(temp_files[temp_count]);
            //printf("File smaller than header.\n");
            break;
        }

        int number_of_entries = temp_files[temp_count]->header->number_of_point_records;
        if (number_of_entries > 2048) {
            printf("Too many entries is header, corrupted file.");
            free(temp_files[temp_count]->header);
            free(temp_files[temp_count]);
            break;
        }
        temp_files[temp_count]->entries = (LASEntry*)malloc(number_of_entries*sizeof(LASEntry));
        for (int i = 0; i < number_of_entries; ++i) {
            return_value = fread(&(temp_files[temp_count]->entries[i]), sizeof(LASEntry), 1, fid);
            if (return_value != 1) {
                free(temp_files[temp_count]->entries);
                free(temp_files[temp_count]->header);
                free(temp_files[temp_count]);
                if (*las_files != NULL) {
                    free (*las_files);
                }
                //printf("File too small.\n");
                break;
            }
        }
        temp_count += 1;
        number_of_files += 1;

        if (temp_count  >= 1024) {
            LASFile ** temp;
            temp = (LASFile**)malloc(number_of_files*sizeof(LASFile*));
            if (*las_files != NULL) {
                memcpy(temp, *las_files, (number_of_files - temp_count) * sizeof(LASFile**));
            }
            memcpy(&(temp[number_of_files-temp_count]), temp_files, temp_count * sizeof(LASFile**));
            temp_count = 0;
            if (*las_files != NULL) {
                free (*las_files);
            }
            *las_files = temp;
        }
    }
    LASFile ** temp;
    temp = (LASFile**)malloc(number_of_files*sizeof(LASFile*));
    if (*las_files != NULL) {
        memcpy(temp, *las_files, (number_of_files - temp_count) * sizeof(LASFile**));
    }
    memcpy(&(temp[number_of_files - temp_count]), temp_files, temp_count * sizeof(LASFile**));
    if (*las_files != NULL) {
        free (*las_files);
    }
    *las_files = temp;

    fclose(fid);
    return number_of_files;
}

LASHeader * initLASHeader (uint64_t utc_time_us, uint32_t number_of_points) {

    LASHeader * return_header = (LASHeader *) malloc(sizeof (LASHeader));

    if (!return_header) {
        return NULL;
    }

    uint64_t adj_pps_time = (uint64_t)(UTCTimeusToAdjustedGPSTime(utc_time_us));

    return_header->file_signature[0] = 'L';
    return_header->file_signature[1] = 'A';
    return_header->file_signature[2] = 'S';
    return_header->file_signature[3] = 'F';
    return_header->file_source_id = 0;
    return_header->global_encoding = 1; //We always use adjusted GPS time
    return_header->guid_data_1 = 0;
    return_header->guid_data_2 = 0;
    return_header->guid_data_3 = 0;
    return_header->guid_data_4 = adj_pps_time;
    return_header->version_major = 1;
    return_header->version_minor = 2;
    memset(return_header->system_identifier, 0, sizeof(return_header->system_identifier));
    memset(return_header->generating_software, 0, sizeof(return_header->generating_software));
    return_header->file_creation_day_of_year = 0;
    return_header->file_creation_year = 0;
    return_header->header_size = HEADER_SIZE;
    return_header->offset_to_point_data = HEADER_SIZE;
    return_header->number_of_variable_length_records = 0;
    return_header->point_data_format_id = 1;
    return_header->point_data_record_length = (uint16_t)(ENTRY_SIZE);
    return_header->number_of_point_records = number_of_points;
    memset(return_header->number_of_points_by_return, 0, sizeof(return_header->number_of_points_by_return));
    return_header->x_scale_factor = header_scale;
    return_header->y_scale_factor = header_scale;
    return_header->z_scale_factor = header_scale;
    return_header->x_offset = 0.0;
    return_header->y_offset = 0.0;
    return_header->z_offset = 0.0;
    return_header->max_x = 0.0;
    return_header->min_x = 0.0;
    return_header->max_y = 0.0;
    return_header->min_y = 0.0;
    return_header->max_z = 0.0;
    return_header->min_z = 0.0;


    return return_header;
}

LASEntry initLASEntry (uint64_t utc_time, double x, double y, double z, uint16_t intensity, uint8_t quality) {
    LASEntry  new_entry;

    new_entry.x = (uint32_t)(x * point_scale);
    new_entry.y = (uint32_t)(y * point_scale);
    new_entry.z = (uint32_t)(z * point_scale);
    new_entry.intensity = intensity;
    new_entry.bit_field = 0;
    new_entry.classification = 0;
    new_entry.scan_angle = 0;
    new_entry.user_data = quality;
    new_entry.point_source_id = 0;
    new_entry.gps_time = UTCTimeusToAdjustedGPSTime(utc_time);
    

    return new_entry;
}


uint64_t AdjustedGPSTimeusToUTCTimeus(uint64_t adj_pps_time) {
    const uint64_t gps_offset = (uint64_t)(18*1E6); // 2017 value

    uint64_t utc_time = diff_to_gps_epoch + (uint64_t)(adj_pps_time) -
                        gps_offset;         // epoch difference + continueing adjustements
    utc_time += 1000000000ull * 1000000ull; // adjusted time offset 10^9 seconds;

    return utc_time;
}

double UTCTimeusToAdjustedGPSTime(uint64_t utc_time) {

    uint32_t offsetSeconds = 18;

    double adjusted_time = (double)(utc_time- diff_to_gps_epoch);

    adjusted_time /= 1000000; // to seconds
    adjusted_time += offsetSeconds;
    adjusted_time -= 1000000000; // adjusted time subtracts 10^9 seconds;

    return adjusted_time;
}



