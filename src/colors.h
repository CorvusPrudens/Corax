
#ifndef COLORS_H
#define COLORS_H

#include <string>

using std::string;

struct Colors {
  // Reset
  inline static string Stop="\033[0m";      // Text Reset

  // Regular Colors
  inline static string Black="\033[0;30m";       // Black
  inline static string Red="\033[0;31m";         // Red
  inline static string Redl="\033[0;91m";        // Light red
  inline static string Green="\033[0;32m";       // Green
  inline static string Yellow="\033[0;33m";      // Yellow
  inline static string Yellowl="\033[0;93m";     // Light yellow
  inline static string Blue="\033[0;34m";        // Blue
  inline static string Bluel="\033[0;94m";       // Light blue
  inline static string Purple="\033[0;35m";      // Purple
  inline static string Cyan="\033[0;36m";        // Cyan
  inline static string White="\033[0;37m";       // White

  inline static string Gray="\033[0;2m";

  // Bold
  inline static string BBlack="\033[1;30m";      // Black
  inline static string BRed="\033[1;31m";        // Red
  inline static string BGreen="\033[1;32m";      // Green
  inline static string BYellow="\033[1;33m";     // Yellow
  inline static string BBlue="\033[1;34m";       // Blue
  inline static string BPurple="\033[1;35m";     // Purple
  inline static string BCyan="\033[1;36m";       // Cyan
  inline static string BWhite="\033[1;37m";      // White

  // Underline
  inline static string UBlack="\033[4;30m";      // Black
  inline static string URed="\033[4;31m";        // Red
  inline static string UGreen="\033[4;32m";      // Green
  inline static string UYellow="\033[4;33m";     // Yellow
  inline static string UBlue="\033[4;34m";       // Blue
  inline static string UPurple="\033[4;35m";     // Purple
  inline static string UCyan="\033[4;36m";       // Cyan
  inline static string UWhite="\033[4;37m";      // White

  // Background
  inline static string On_Black="\033[40m";      // Black
  inline static string On_Red="\033[41m";        // Red
  inline static string On_Green="\033[42m";      // Green
  inline static string On_Yellow="\033[43m";     // Yellow
  inline static string On_Blue="\033[44m";       // Blue
  inline static string On_Purple="\033[45m";     // Purple
  inline static string On_Cyan="\033[46m";       // Cyan
  inline static string On_White="\033[47m";      // White

  // High Intensity
  inline static string IBlack="\033[0;90m";      // Black
  inline static string IRed="\033[0;91m";        // Red
  inline static string IGreen="\033[0;92m";      // Green
  inline static string IYellow="\033[0;93m";     // Yellow
  inline static string IBlue="\033[0;94m";       // Blue
  inline static string IPurple="\033[0;95m";     // Purple
  inline static string ICyan="\033[0;96m";       // Cyan
  inline static string IWhite="\033[0;97m";      // White

  // Bold High Intensity
  inline static string BIBlack="\033[1;90m";     // Black
  inline static string BIRed="\033[1;91m";       // Red
  inline static string BIGreen="\033[1;92m";     // Green
  inline static string BIYellow="\033[1;93m";    // Yellow
  inline static string BIBlue="\033[1;94m";      // Blue
  inline static string BIPurple="\033[1;95m";    // Purple
  inline static string BICyan="\033[1;96m";      // Cyan
  inline static string BIWhite="\033[1;97m";     // White

  // High Intensity backgrounds
  inline static string On_IBlack="\033[0;100m";  // Black
  inline static string On_IRed="\033[0;101m";    // Red
  inline static string On_IGreen="\033[0;102m";  // Green
  inline static string On_IYellow="\033[0;103m"; // Yellow
  inline static string On_IBlue="\033[0;104m";   // Blue
  inline static string On_IPurple="\033[0;105m"; // Purple
  inline static string On_ICyan="\033[0;106m";   // Cyan
  inline static string On_IWhite="\033[0;107m";  // White

};

#endif // COLORS_H