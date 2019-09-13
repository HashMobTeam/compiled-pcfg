//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Pretty Cool Fuzzy Guesser (PCFG)
//  --Probabilistic Context Free Grammar (PCFG) Password Guessing Program
//
//  Written by Matt Weir
//  Special thanks to the John the Ripper and Hashcat communities where some 
//  of the code was copied from. And thank you whoever is reading this. Be good!
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
//

#include "grammar_io.h"


// Checks the version the ruleset was created and makes sure it is supported
//
// Function returns a non-zero value if an error occurs
//     1 = problem opening the file
//     2 = malformed ruleset
//     3 = unsupported feature/ruleset
//
int check_encoding(char *config_filename) {
    
    char result[MAX_CONFIG_LINE];
    
    int ret_value = get_key(config_filename, "TRAINING_DATASET_DETAILS", "encoding", result);
    
    switch (ret_value) {
        // Version was found
        case 0:
            break;
        case 1:
            fprintf(stderr, "Error opening config file:%s\n",config_filename);
            return 1;
        case 2:
            fprintf(stderr, "Malformed config file:%s\n",config_filename);
            return 2;
        default:
            return ret_value;
    }
    
    // It is utf-8 or ASCII encoding so it is supported
    if ((strncmp(result, "utf-8", MAX_CONFIG_LINE) == 0) || (strncmp(result,"ascii", MAX_CONFIG_LINE) == 0)){
        return 0;
    }
    
    fprintf(stderr, "Unfortunatly, only UTF-8 or ASCII rulesets are currently supported by the compiled pcfg_guesser\n");
    fprintf(stderr, "Note: The Python pcfg_guesser supports other encoding schemes\n");
    fprintf(stderr, "Detected Encodign: %s\n", result);
    return 3;
}

// Splits a terminal file into a value, prob pair
//
// Input: Note, memory needs to be allocated by the calling program
//
//     input: the string that needs to be split_value
//
//     value: contains the return value string (must be allocated by calling program)
//
//     prob: contains the probability from the split value (must be allocated by calling program)
//
// Function returns a non-zero value if an error occurs
//     1 = Problem splitting the input
//
int split_value(char *input, char *value, double *prob) {
    
    //find the split point
    char *split_point = strchr(input,'\t');
        
    //If there isn't a tab to split the input up
    if (split_point == NULL) {
        return 1;
    }
        
    (*prob) = strtod(split_point,NULL);
        
    // Check to make sure it was a number
    if ((errno == EINVAL) || (errno == ERANGE))
    {
        fprintf(stderr, "Invalid probability found. Exiting\n");
        return 1;
    }
     
    // Make sure the id falls within the acceptable range
    if ( ((*prob) < 0.0) || ((*prob) > 1.0) ) {
        fprintf(stderr, "Invalid probability found in rules. Exiting\n");
        return 1;
    }      

    //Assign the value
    strncpy(value, input, split_point-input);
    value[split_point-input] = '\0';

    //print the length of the first items
    //printf("Length:%li Value:%s Prob: %E\n",split_point-input, value, (*prob));
    
    //char str[100];
    //int result = scanf("%s", str);
    
    return 0;
}

// Opens a file and actually loads the grammar for a particular terminal
//
// Function returns a non-zero value if an error occurs
//     1 = problem opening the file or malformed ruleset
//
int load_term_from_file(char *filename, PcfgReplacements *terminal_pointer) {
    
    // Pointer to the open terminal file
    FILE *fp;
    
    // Open the terminal file
    fp= fopen(filename,"r");
    
    // Check to make sure the file opened correctly
    if (fp== NULL) {
        
        //Could not open the file. Print error and return an error
        fprintf(stderr, "Error. Could not read the file: %s\n",filename);
        return 1;
    }
    
    // Note: There will be two passes through the file
    //   1) First pass gets count info since I want to allocate memory without
    //      being wasteful, and I want to use arrays to hopefully make
    //      generating guesses faster
    //
    //   2) Actually load the terminals to the pcfgreplacement struct
    //
    
    // This contains the number of items at the current probability
    int num_term = 0;
    
    // Holds the current line in the config file
    char buff[MAX_CONFIG_LINE];
    
    // Holds the temporary values
     
    // Loop through the config file
    while (fgets(buff, MAX_CONFIG_LINE , (FILE*)fp)) {
        double prob;
        char value[MAX_CONFIG_LINE];
        
        if (split_value(buff, value, &prob) != 0) {
            return 1;
        }
        
    }
    
    return 0;
}


// Loads the grammar for a particular terminal
//
// Function returns a non-zero value if an error occurs
//     1 = problem opening the file or malformed ruleset
//
int load_terminal(char *config_filename, char *base_directory, char *structure, PcfgReplacements *grammar_item[]) {
    
    // Get the folder where the files will be saved
    char section_folder[MAX_CONFIG_LINE];
    
    if (get_key(config_filename, structure, "directory", section_folder) != 0) {
        fprintf(stderr, "Could not get folder name for section. Exiting\n");
        return 1;
    }
    
    // Get the filenames associated with the structure
    char result[256][MAX_CONFIG_ITEM];
    int list_size;
    if (config_get_list(config_filename, structure, "filenames", result, &list_size, 256) != 0) {
        fprintf(stderr, "Error reading the config for a rules file. Exiting\n");
        return 1;
	}
    
    //Load each of the files
    for (int i = 0; i< list_size; i++) {

        //Find the id for this file, aka A1 vs A23. This is the 1, or 23
        char *end_pos = strchr(result[i],'.');
        
        //If there isn't a .txt, this is an invalid file
        if (end_pos == NULL) {
            return 1;
        }
        
        long id = strtol(result[i],&end_pos, 10);
        
        // Check to make sure it was a number
        if ((errno == EINVAL) || (errno == ERANGE))
        {
            fprintf(stderr, "Invalid File name found in rules. Exiting\n");
            return 1;
        }
         
        // Make sure the id falls within the acceptable range
        if (id <= 0) {
            fprintf(stderr, "Invalid File name found in rules. Exiting\n");
            return 1;
        }      

        // Make sure the value isn't too long for the current compiled pcfg_guesser
        if (id > MAX_TERM_LENGTH) {
            continue;
        }
        
        char filename[PATH_MAX];
        snprintf(filename, PATH_MAX, "%s%s%c%s", base_directory,section_folder,SLASH,result[i]);
        printf("%s\n",filename);
        if (load_term_from_file(filename, grammar_item[id]) != 0) {
            return 1;
        }

    }

    return 0;
}


// Loads a ruleset/grammar from disk
//
// Function returns a non-zero value if an error occurs
//     1 = problem opening the file
//     2 = malformed ruleset
//     3 = unsupported feature/ruleset
//
int load_grammar(char *arg_exec, struct program_info program_info, PcfgGrammar *pcfg) {
    
    // Directory the executable is running in
    // Adding in a plus 1 to PATH_MAX to deal with a potential edge case
    // when formatting this to represent the directory. Should never happen
    // but still...
    char exec_directory[PATH_MAX + 1];
    strncpy(exec_directory, arg_exec, PATH_MAX);
    
    // Hate doing it this way but haven't found a good way to get the directory
    char *tail_slash = strrchr(exec_directory, SLASH);
    
    // Holds the return value of function calls
    int ret_value;
    
    // Using the current directory
    if (tail_slash == NULL) {
        snprintf(exec_directory, PATH_MAX, ".%c", SLASH);
    }
    // Using a directory passed in via argv[0]
    else {   
        tail_slash[1]= '\0';
    }
    
    // Create the base directory to load the rules from
    char base_directory[FILENAME_MAX];
    snprintf(base_directory, FILENAME_MAX, "%sRules%c%s%c", exec_directory,  SLASH, program_info.rule_name, SLASH);
    
    fprintf(stderr, "Loading Ruleset:%s\n",base_directory);
    
    // Save the config file name for easy reference
    char config_filename[FILENAME_MAX];
    snprintf(config_filename, FILENAME_MAX, "%sconfig.ini", base_directory);
    
    // Currently we only support UTF-8 for the compiled version, so ensure
    // the ruleset uses that
    //
    // Note: The Python version of the pcfg_guesser supports other encoding
    //       types.
    ret_value = check_encoding(config_filename);
    if (ret_value != 0) {
        return ret_value;
    }
    
    // Read the rules filelength
    char result[256][MAX_CONFIG_ITEM];
    int list_size;
    if (load_terminal(config_filename, base_directory, "BASE_A", pcfg->a) != 0) {
        fprintf(stderr, "Error reading the rules file. Exiting\n");
        return 1;
	}
    
    return 0;
}