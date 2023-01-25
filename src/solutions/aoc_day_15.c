#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "aoc_solutions.h"
#include "aoc_day_15.h"
#include "file_utils.h"

static void determine_distance_apart(day_15_sensor_t * sensor)
{
    sensor->distance_apart = labs(sensor->sensor_x - sensor->beacon_x) + labs(sensor->sensor_y - sensor->beacon_y);
#ifdef DEBUG_DAY_15
    printf("Sensor (%ld,%ld) and beacon (%ld,%ld) are %ld units apart\n", sensor->sensor_x, sensor->sensor_y, sensor->beacon_x, sensor->beacon_y, sensor->distance_apart);
#endif
}

static void calculate_target_row_exclude(day_15_sensor_t * sensor, long target_row)
{
    long sensor_to_target_rows = labs(sensor->sensor_y - target_row);
#ifdef DEBUG_DAY_15
    printf("Sensor (%ld,%ld) is %ld units apart from the target row %ld\n", sensor->sensor_x, sensor->sensor_y, sensor_to_target_rows, target_row);
#endif
    if (sensor_to_target_rows > sensor->distance_apart)
    {
#ifdef DEBUG_DAY_15
        printf(" This is too far; can only be %ld units away. Does not match any in the target row\n", sensor->distance_apart);
#endif
        sensor->covers_target_row = FALSE;
        return;
    }
    sensor->covers_target_row = TRUE;
    sensor->min_col_in_taget_row_covered = sensor->sensor_x - (sensor->distance_apart - sensor_to_target_rows);
    sensor->max_col_in_taget_row_covered = sensor->sensor_x + (sensor->distance_apart - sensor_to_target_rows);
#ifdef DEBUG_DAY_15
    printf(" This can cover positions %ld thru %ld in the target row\n", sensor->min_col_in_taget_row_covered, sensor->max_col_in_taget_row_covered);
#endif
    return;
}

static void calculate_min_max_rows_covered(day_15_sensor_t * sensor)
{
    sensor->min_row_covered = sensor->sensor_y - sensor->distance_apart;
    sensor->max_row_covered = sensor->sensor_y + sensor->distance_apart;
#ifdef DEBUG_DAY_15
    printf(" Sensor will cover rows from %ld to %ld\n", sensor->min_row_covered, sensor->max_row_covered);
#endif
    return;
}

static long calculate_number_excluded(day_15_sensors_t * sensors, long target_row)
{
    long sum = 0;
    int beacon_on_row = FALSE;

#ifdef DEBUG_DAY_15
    printf("Calculating total sum of of excluded values\n");
#endif

    for (int i=0; i<sensors->num_sensors; i++)
    {
        if (sensors->sensors[i].covers_target_row == TRUE)
        {
            if (sensors->sensors[i].beacon_y == target_row)
            {
#ifdef DEBUG_DAY_15
                printf(" Sensor (%ld,%ld) has beacon on target row\n", 
                   sensors->sensors[i].sensor_x, sensors->sensors[i].sensor_y);
#endif        
                beacon_on_row = TRUE;
            }
        }
    }
    
    long last_max = LONG_MIN;
    while (1)
    {
        int found_range = FALSE;
        long current_min = LONG_MAX;
        long current_max;
#ifdef DEBUG_DAY_15
        printf("Checking for a range where last_max is %ld\n", last_max);
#endif
        // find the lowest min greater than last_max
        for (int i=0; i<sensors->num_sensors; i++)
        {
            if (sensors->sensors[i].covers_target_row == TRUE && 
                sensors->sensors[i].min_col_in_taget_row_covered > last_max &&
                sensors->sensors[i].min_col_in_taget_row_covered < current_min)
            {
#ifdef DEBUG_DAY_15
                printf(" Found range from %ld-%ld that has new lowest min value greater than last_max\n", sensors->sensors[i].min_col_in_taget_row_covered, sensors->sensors[i].max_col_in_taget_row_covered);
#endif
                current_min = sensors->sensors[i].min_col_in_taget_row_covered;
                current_max = sensors->sensors[i].max_col_in_taget_row_covered;
                found_range = TRUE;
            }
        }

        if (found_range == FALSE)
        {
#ifdef DEBUG_DAY_15
            printf(" No range found. Done adding to sum\n");
#endif
            break;
        }
        
        int expanded_range;
        do
        {
            expanded_range = FALSE;
            for (int i=0; i<sensors->num_sensors; i++)
            {
                if (sensors->sensors[i].covers_target_row == TRUE && 
                    sensors->sensors[i].min_col_in_taget_row_covered >= current_min && 
                    sensors->sensors[i].min_col_in_taget_row_covered <= current_max)
                {
                    if (sensors->sensors[i].max_col_in_taget_row_covered > current_max)
                    {
                        current_max = sensors->sensors[i].max_col_in_taget_row_covered;
#ifdef DEBUG_DAY_15
                        printf(" Range from %ld-%ld expands max; new current range is %ld-%ld\n", sensors->sensors[i].min_col_in_taget_row_covered, sensors->sensors[i].max_col_in_taget_row_covered, current_min, current_max);
#endif
                        expanded_range = TRUE;
                    }
                }
            }
        } while (expanded_range == TRUE);
        
        sum += (current_max - current_min + 1);
        
#ifdef DEBUG_DAY_15
        printf("Added %ld to sum, increasing its value to %ld\n", current_max - current_min + 1, sum);
#endif
        last_max = current_max;
    }
    
    if (beacon_on_row == TRUE)
    {
        sum -=1;
#ifdef DEBUG_DAY_15
        printf("Decreasing sum by 1 due to beacon on row results in sum of %ld\n", sum);
#endif        
    }
    return sum;
}

static int is_row_complete(day_15_sensor_group_t * sensor_group, long row, long * missing_col, long max_checked_col)
{
    for (int i=0; i<sensor_group->num_sensors; i++)
    {
        day_15_sensor_t * sensor = sensor_group->sensors[i];
        sensor->min_col_in_taget_row_covered = sensor->sensor_x - (sensor->distance_apart - labs(sensor->sensor_y - row));
        sensor->max_col_in_taget_row_covered = sensor->sensor_x + (sensor->distance_apart - labs(sensor->sensor_y - row));
#ifdef DEBUG_DAY_15
        printf("  Sensor at (%ld,%ld) with distance apart %ld has min_col %ld and max_col %ld\n", sensor->sensor_x, sensor->sensor_y, sensor->distance_apart, sensor->min_col_in_taget_row_covered, sensor->max_col_in_taget_row_covered);
#endif        
    }
    
    long last_max = LONG_MIN;
    long minimums[5];
    long maximums[5];
    int current_min_max_idx = 0;
    while (1)
    {
        int found_range = FALSE;
        minimums[current_min_max_idx] = LONG_MAX;
#ifdef DEBUG_DAY_15
        printf("  Checking for a range where last_max is %ld with current_min_max_idx=%d\n", last_max, current_min_max_idx);
#endif
        // find the lowest min greater than last_max
        for (int i=0; i<sensor_group->num_sensors; i++)
        {
            if (sensor_group->sensors[i]->min_col_in_taget_row_covered > last_max &&
                sensor_group->sensors[i]->min_col_in_taget_row_covered < minimums[current_min_max_idx])
            {
#ifdef DEBUG_DAY_15
                printf("   Found range from %ld-%ld that has new lowest min value greater than last_max\n", sensor_group->sensors[i]->min_col_in_taget_row_covered, sensor_group->sensors[i]->max_col_in_taget_row_covered);
#endif
                minimums[current_min_max_idx] = sensor_group->sensors[i]->min_col_in_taget_row_covered;
                maximums[current_min_max_idx] = sensor_group->sensors[i]->max_col_in_taget_row_covered;
                found_range = TRUE;
            }
        }

        if (found_range == FALSE)
        {
#ifdef DEBUG_DAY_15
            printf("   No range found. Done partitioning\n");
#endif
            break;
        }
        
        int expanded_range;
        do
        {
            expanded_range = FALSE;
            for (int i=0; i<sensor_group->num_sensors; i++)
            {
                if (sensor_group->sensors[i]->min_col_in_taget_row_covered >= minimums[current_min_max_idx] && 
                    sensor_group->sensors[i]->min_col_in_taget_row_covered <= (maximums[current_min_max_idx] + 1))
                {
                    if (sensor_group->sensors[i]->max_col_in_taget_row_covered > maximums[current_min_max_idx])
                    {
                        maximums[current_min_max_idx] = sensor_group->sensors[i]->max_col_in_taget_row_covered;
#ifdef DEBUG_DAY_15
                        printf("   Range from %ld-%ld expands max; new current range is %ld-%ld\n", sensor_group->sensors[i]->min_col_in_taget_row_covered, sensor_group->sensors[i]->max_col_in_taget_row_covered, minimums[current_min_max_idx], maximums[current_min_max_idx]);
#endif
                        expanded_range = TRUE;
                    }
                }
            }
        } while (expanded_range == TRUE);
        
        last_max = maximums[current_min_max_idx];
        current_min_max_idx++;
    }
    
    if (current_min_max_idx == 1)
    {
        if (minimums[0] <= 0 && maximums[0] >= max_checked_col)
        {
#ifdef DEBUG_DAY_15
            printf(" One good range found. This row is completely excluded\n");
#endif
            return TRUE;
        }
    }
    else if (current_min_max_idx == 2)
    {
        if (minimums[0] <= 0 && maximums[1] >= max_checked_col && (minimums[1] - maximums[0] == 2))
        {
#ifdef DEBUG_DAY_15
            printf(" Two ranges with one empty cell found. Empty column is at %ld\n", maximums[0]+1);
#endif
            *missing_col = maximums[0]+1;
            return FALSE;
        }
    }
    
    fprintf(stderr, "!!!!Need more logic in is_row_complete for row %ld!!!!!\n", row);
    return TRUE;
}


static int parse_input(char * filename, day_15_sensors_t * sensors)
{
    file_data_t fd;
    line_data_t * ld;
    token_data_t * td;
    
    // read in the input file with =,: delimiters
    file_data_init(&fd);
    file_data_read_file(&fd, filename, "=,:", 3, '\0', '\0');
    if (fd.num_lines == 0)
    {
        fprintf(stderr, "Error reading in data from %s\n", filename);
        file_data_cleanup(&fd);
        return FALSE;
    }

    ld = fd.head_line;
    td = ld->head_token;
    
    sensors->num_sensors = 0;
    
    // Sensor at x=2, y=18: closest beacon is at x=-2, y=15
    // [Sensor at x]=[2],[y]=[18]:[ closest beacon is at x]=[-2],[ y]=[15]
    while (ld != NULL)
    {
        td = ld->head_token;
        // skip position 1
        td = td->next;
        
        // sensor_x at position 2
        sensors->sensors[sensors->num_sensors].sensor_x = strtol(td->token, NULL, 10);
        td = td->next;
        
        // skip position 3
        td = td->next;
        
        // sensor_y at position 4
        sensors->sensors[sensors->num_sensors].sensor_y = strtol(td->token, NULL, 10);
        td = td->next;
        
        // skip position 5
        td = td->next;
        
        // beacon_x at position 7
        sensors->sensors[sensors->num_sensors].beacon_x = strtol(td->token, NULL, 10);
        td = td->next;
        
        // skip position 7
        td = td->next;
        
        // beacon_y at position 8
        sensors->sensors[sensors->num_sensors].beacon_y = strtol(td->token, NULL, 10);
        
        
#ifdef DEBUG_DAY_15
        printf("Row %d has sensor at (%ld,%ld) and beacon at (%ld,%ld)\n", 
               sensors->num_sensors, sensors->sensors[sensors->num_sensors].sensor_x, sensors->sensors[sensors->num_sensors].sensor_y,
               sensors->sensors[sensors->num_sensors].beacon_x, sensors->sensors[sensors->num_sensors].beacon_y);
#endif

        sensors->num_sensors++;
        ld = ld->next;
    }

    return TRUE;
}

static void create_sensor_groups(day_15_sensor_groups_t * sensor_groups, day_15_sensors_t * sensors, long max_row_col)
{
    long group_start_values[DAY_15_MAX_SENSORS*2];
    long temp_values[DAY_15_MAX_SENSORS*2+2];
    int num_temp_values = sensors->num_sensors*2;
    int num_start_values = 0;
    
    for (int i=0; i<sensors->num_sensors; i++)
    {
        temp_values[i*2]=sensors->sensors[i].min_row_covered;
        temp_values[i*2+1]=sensors->sensors[i].max_row_covered+1;
#ifdef DEBUG_DAY_15
        printf("Added %ld and %ld to temp_values from sensor %d\n", temp_values[i*2], temp_values[i*2+1], i);
#endif
    }
    
    temp_values[num_temp_values] = 0;
    temp_values[num_temp_values+1] = max_row_col+1;
    num_temp_values+=2;
#ifdef DEBUG_DAY_15
    printf("Added 0 and %ld to temp_values as limits\n", max_row_col+1);
#endif
    
    while (1)
    {
#ifdef DEBUG_DAY_15
        printf("Setting position %d in group_start_values\n", num_start_values);
#endif
        group_start_values[num_start_values] = LONG_MAX;
        for (int i=0; i<num_temp_values; i++)
        {
            if ( (num_start_values == 0 || 
                  temp_values[i] > group_start_values[num_start_values-1]) && 
                temp_values[i] < group_start_values[num_start_values])
            {
                group_start_values[num_start_values] = temp_values[i];
#ifdef DEBUG_DAY_15
                printf(" Assigning %ld\n", group_start_values[num_start_values]);
#endif
            }
        }
        if (group_start_values[num_start_values] == LONG_MAX)
        {
#ifdef DEBUG_DAY_15
            printf(" Nothing set; breaking and not incrementing counter\n");
#endif
            break;
        }
#ifdef DEBUG_DAY_15
        printf(" Final value %ld; incrementing counter\n", group_start_values[num_start_values]);
#endif
        num_start_values++;
    }
    
#ifdef DEBUG_DAY_15
    printf("Making groups\n");
#endif
    sensor_groups->num_sensor_groups = 0;
    for (int i=0; i<num_start_values; i++)
    {
        if (group_start_values[i] < 0 || group_start_values[i] > max_row_col)
        {
#ifdef DEBUG_DAY_15
            printf("Skipping group with start value %ld\n", group_start_values[i]);
#endif
            continue;
        }
#ifdef DEBUG_DAY_15
        printf("Creating group %d with start value %ld and end value %ld\n", sensor_groups->num_sensor_groups, group_start_values[i], group_start_values[i+1]-1);
#endif
        day_15_sensor_group_t * current_group = &sensor_groups->sensor_groups[sensor_groups->num_sensor_groups];
        current_group->min_row = group_start_values[i];
        current_group->max_row = group_start_values[i+1]-1;
        current_group->num_sensors = 0;
        for (int j=0; j<sensors->num_sensors; j++)
        {
            if (sensors->sensors[j].min_row_covered > current_group->max_row)
            {
#ifdef DEBUG_DAY_15
                printf(" Sensor %d from rows %ld to %ld is after this group. skipping\n", j, sensors->sensors[j].min_row_covered, sensors->sensors[j].max_row_covered);
#endif
            }
            else if (sensors->sensors[j].max_row_covered < current_group->max_row)
            {
#ifdef DEBUG_DAY_15
                printf(" Sensor %d from rows %ld to %ld is before this group. skipping\n", j, sensors->sensors[j].min_row_covered, sensors->sensors[j].max_row_covered);
#endif
            }
            else
            {
#ifdef DEBUG_DAY_15
                printf(" Sensor %d from rows %ld to %ld is in this group. Including at group position %d\n", j, sensors->sensors[j].min_row_covered, sensors->sensors[j].max_row_covered, current_group->num_sensors);
#endif
                current_group->sensors[current_group->num_sensors]=&sensors->sensors[j];
                current_group->num_sensors++;
            }
        }
        sensor_groups->num_sensor_groups++;
    }
    
}

void day_15_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    if (extra_args->num_extra_args != 1)
    {
        fprintf(stderr, "One extra argument for the target row is required for Day 15 Part 1\n");
        return;
    }

    long target_row = strtol(extra_args->extra_args[0], NULL, 10);
    
    printf("Target row for Day 15 Part 1 is %ld\n", target_row);
    
    day_15_sensors_t sensors;
    if (parse_input(filename, &sensors) != TRUE)
    {
        fprintf(stderr, "Error parsing input %s\n", filename);
    }
    
    for (int i=0; i<sensors.num_sensors; i++)
    {
        determine_distance_apart(&sensors.sensors[i]);
        calculate_target_row_exclude(&sensors.sensors[i], target_row);
    }
    
    long total_excluded = calculate_number_excluded(&sensors, target_row);
    
    snprintf(result, MAX_RESULT_LENGTH+1, "%ld", total_excluded);
    
    return;
}

void day_15_part_2(char * filename, extra_args_t * extra_args, char * result)
{
    if (extra_args->num_extra_args != 1)
    {
        fprintf(stderr, "One extra argument for the upper limit of coorindates to search is required for Day 15 Part 1\n");
        return;
    }

    long max_row_col = strtol(extra_args->extra_args[0], NULL, 10);
    
    printf("Will be searching from 0 to %ld for Day 15 Part 2\n", max_row_col);
    
    day_15_sensors_t sensors;
    day_15_sensor_groups_t sensor_groups;
    
    if (parse_input(filename, &sensors) != TRUE)
    {
        fprintf(stderr, "Error parsing input %s\n", filename);
    }
    
    for (int i=0; i<sensors.num_sensors; i++)
    {
        determine_distance_apart(&sensors.sensors[i]);
        calculate_min_max_rows_covered(&sensors.sensors[i]);
    }
    
    create_sensor_groups(&sensor_groups, &sensors, max_row_col);
    
    long missing_col = LONG_MIN;
    long tuning_frequency = LONG_MIN;
    
    for (int i=0; i<sensor_groups.num_sensor_groups; i++)
    {
#ifdef DEBUG_DAY_15
        printf("Checking sensor group %d - rows %ld to %ld\n", i, sensor_groups.sensor_groups[i].min_row, sensor_groups.sensor_groups[i].max_row);
#endif
        for (long row=sensor_groups.sensor_groups[i].min_row; row<=sensor_groups.sensor_groups[i].max_row; row++)
        {
#ifdef DEBUG_DAY_15
            printf(" Checking row %ld\n", row);
#endif
            if (is_row_complete(&sensor_groups.sensor_groups[i], row, &missing_col, max_row_col) == FALSE)
            {
                tuning_frequency = (4000000L * missing_col) + row;
                printf("Missing column at row %ld column %ld results in tuning frequency of %ld\n", row, missing_col, tuning_frequency);
            }
        }
    }
    
    snprintf(result, MAX_RESULT_LENGTH+1, "%ld", tuning_frequency);
    
    return;
}
