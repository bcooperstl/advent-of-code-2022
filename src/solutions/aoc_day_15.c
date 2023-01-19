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
    sensor->min_target_row_covered = sensor->sensor_x - (sensor->distance_apart - sensor_to_target_rows);
    sensor->max_target_row_covered = sensor->sensor_x + (sensor->distance_apart - sensor_to_target_rows);
#ifdef DEBUG_DAY_15
    printf(" This can cover positions %ld thru %ld in the target row\n", sensor->min_target_row_covered, sensor->max_target_row_covered);
#endif
    return;
}

long calculate_overlap(day_15_sensor_t * first, day_15_sensor_t * second)
{
#ifdef DEBUG_DAY_15
    printf("Calculating overlap for sensors at (%ld,%ld) with range %ld thru %ld and (%ld,%ld) with range %ld thru %ld\n", 
            first->sensor_x, first->sensor_y, first->min_target_row_covered, first->max_target_row_covered,
            second->sensor_x, second->sensor_y, second->min_target_row_covered, second->max_target_row_covered);
#endif

    day_15_sensor_t * low;
    day_15_sensor_t * high;

    if (first->min_target_row_covered <= second->min_target_row_covered)
    {
        low = first;
        high = second;
    }
    else
    {
        low = second;
        high = first;
    }
#ifdef DEBUG_DAY_15
    printf(" Using low sensor range %ld thru %ld and high sensor range %ld thru %ld\n", 
            low->min_target_row_covered, low->max_target_row_covered,
            high->min_target_row_covered, high->max_target_row_covered);
#endif
    
    if (high->min_target_row_covered > low->max_target_row_covered) // no overlap
    {
#ifdef DEBUG_DAY_15
        printf(" High range starts after the low range ends. No overlap\n");
#endif
        return 0;
    }
    if (high->max_target_row_covered < low->max_target_row_covered) // high fully inside low
    {
#ifdef DEBUG_DAY_15
        printf(" High range fully inside low range. Overlap is size of high range %ld\n", high->max_target_row_covered - high->min_target_row_covered + 1);
#endif
        return high->max_target_row_covered - high->min_target_row_covered + 1;
    }
    // high goes past low
#ifdef DEBUG_DAY_15
    printf(" High range goes past end of low range. Calculated size of overlap at %ld\n", low->max_target_row_covered - high->min_target_row_covered + 1);
#endif
    return low->max_target_row_covered - high->min_target_row_covered + 1;
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
            sum += (sensors->sensors[i].max_target_row_covered - sensors->sensors[i].min_target_row_covered + 1);
#ifdef DEBUG_DAY_15
            printf(" Sensor (%ld,%ld) added %ld units, bringing sum to %ld\n", 
                   sensors->sensors[i].sensor_x, sensors->sensors[i].sensor_y, 
                   (sensors->sensors[i].max_target_row_covered - sensors->sensors[i].min_target_row_covered + 1), sum);
#endif        
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
    
    for (int first=0; first<sensors->num_sensors - 1; first++)
    {
        for (int second=first+1; second<sensors->num_sensors; second++)
        {
            if ((sensors->sensors[first].covers_target_row == TRUE) &&
                (sensors->sensors[second].covers_target_row == TRUE))
            {
                long overlap = calculate_overlap(&sensors->sensors[first], &sensors->sensors[second]);
                sum -= overlap;
#ifdef DEBUG_DAY_15
                printf(" Decreasing by overlap of %ld results in sum value of %ld\n", overlap, sum);
#endif        
            }
        }
    }
    if (beacon_on_row == TRUE)
    {
        sum -=1;
#ifdef DEBUG_DAY_15
        printf(" Decreasing sum by 1 due to beacon on row results in sum of %ld\n", sum);
#endif        
    }
    return sum;
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

