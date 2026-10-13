#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>

#include <ANSI-color-codes.h>
#include <palerain.h>

#ifdef TUI
#include <tui.h>
#endif

static FILE *log_file = NULL;
static pthread_mutex_t log_file_mutex = PTHREAD_MUTEX_INITIALIZER;

int log_file_init(const char *path)
{
    if (log_file) {
        fclose(log_file);
        log_file = NULL;
    }
    log_file = fopen(path, "a");
    if (!log_file) {
        return -1;
    }
    return 0;
}

void log_file_cleanup(void)
{
    pthread_mutex_lock(&log_file_mutex);
    if (log_file) {
        fclose(log_file);
        log_file = NULL;
    }
    pthread_mutex_unlock(&log_file_mutex);
}

static void log_to_file(const char *msg)
{
    if (!log_file) return;
    pthread_mutex_lock(&log_file_mutex);
    if (log_file) {
        fprintf(log_file, "%s", msg);
        fflush(log_file);
    }
    pthread_mutex_unlock(&log_file_mutex);
}

int p1_log(log_level_t loglevel, const char *fname, int lineno, const char *fxname, const char *__restrict format, ...)
{
#ifdef TUI
	if (tui_started) return 0;
#endif
    if (verbose >= 5 
#ifdef TUI
	&& !PALERAIN_OPTION_TEST(palerain_flags, palerain_option_tui)
#endif
	) fprintf(stderr, "p1_log: loglevel %d from %s:%d:%s()\n", loglevel, fname, lineno, fxname);
	int ret = 0;
	char type[0x10];
	char colour[0x10];
	char colour_bold[0x10];
	va_list args;
	va_start(args, format);
	if (verbose < (loglevel - 3) && loglevel > LOG_INFO) {
        if (verbose >= 5
#ifdef TUI
		&& !tui_started
#endif
) fprintf(stderr, "p1_log: hid log with high log level (%d < %d)\n", verbose, loglevel - 3);
		va_end(args);
        return 0;
    }
	switch (loglevel) {
	case LOG_FATAL:
		snprintf(type, 0x10, "%s", "!");
		snprintf(colour, 0x10, "%s", RED);
		snprintf(colour_bold, 0x10, "%s", BRED);
		break;
	case LOG_ERROR:
		snprintf(type, 0x10, "%s", "Error");
		snprintf(colour, 0x10, "%s", RED);
		snprintf(colour_bold, 0x10, "%s", BRED);
		break;
	case LOG_WARNING:
		snprintf(type, 0x10, "%s", "Warning");
		snprintf(colour, 0x10, "%s", YEL);
		snprintf(colour_bold, 0x10, "%s", BYEL);
		break;
	case LOG_INFO:
		snprintf(type, 0x10, "%s", "Info");
		snprintf(colour, 0x10, "%s", CYN);
		snprintf(colour_bold, 0x10, "%s", BCYN);
		break;
	default:
		assert(loglevel >= 0);
		snprintf(type, 0x10, "%s", "Verbose");
		snprintf(colour, 0x10, "%s", WHT);
		snprintf(colour_bold, 0x10, "%s", BWHT);
		break;
	}
	if (PALERAIN_OPTION_TEST(palerain_flags, palerain_option_no_colors)) {
		colour[0] = '\0';
		colour_bold[0] = '\0';
	}
	{
		pthread_mutex_lock(&log_mutex);
		char timestring[0x80];
		time_t curtime;
		time(&curtime);
		struct tm* timeinfo = localtime(&curtime);
		if (!PALERAIN_OPTION_TEST(palerain_flags, palerain_option_no_colors))
			snprintf(timestring, 0x80, "%s[%s%02d/%02d/%d %02d:%02d:%02d%s]", CRESET, HBLK, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_year - 100, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, CRESET);
		else
			snprintf(timestring, 0x80, "[%02d/%02d/%d %02d:%02d:%02d]", timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_year - 100, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

		/* Build the log message for file output (without color codes) */
		char file_msg[0x200];
		if (verbose >= 2) {
			snprintf(file_msg, sizeof(file_msg), "| - %s <%s> %s:%d:%s(): \n| ----> ", timestring, type, fname, lineno, fxname);
		} else {
			snprintf(file_msg, sizeof(file_msg), " - %s <%s>: ", timestring, type);
		}

		if (verbose >= 2 && !PALERAIN_OPTION_TEST(palerain_flags, palerain_option_no_colors)) {
			printf("%s| - %s%s <%s> " CRESET "%s" HBLU "%s" CRESET ":" BLU "%d" CRESET ":" BMAG "%s()" CRESET ": \n%s| ----> ", colour_bold, timestring, colour_bold, type, WHT, fname, lineno, fxname, colour_bold);
		} else if (!PALERAIN_OPTION_TEST(palerain_flags, palerain_option_no_colors)) {
			printf(" - %s %s<%s>%s: ", timestring, colour_bold, type, CRESET);
		} else if (verbose >= 2 && PALERAIN_OPTION_TEST(palerain_flags, palerain_option_no_colors)) {
			printf("%s| - %s%s <%s> %s:%d:%s(): \n%s| ----> ", colour_bold, timestring, colour_bold, type, fname, lineno, fxname, colour_bold);
		} else if (PALERAIN_OPTION_TEST(palerain_flags, palerain_option_no_colors)) {
			printf(" - %s %s<%s>: ", timestring, colour_bold, type);
		}
		printf("%s", colour);
		ret = vprintf(format, args);
		va_end(args);

		if (verbose < 2 && !PALERAIN_OPTION_TEST(palerain_flags, palerain_option_no_colors))
			printf(CRESET "\n");
		else if (!PALERAIN_OPTION_TEST(palerain_flags, palerain_option_no_colors))
			printf("\n%s-%s\n", colour, CRESET);
		else if (verbose < 2)
			printf("\n");
		else
			printf("\n%s-\n", colour);
		fflush(stdout);

		/* Write to log file if enabled */
		if (PALERAIN_OPTION_TEST(palerain_flags, palerain_option_log_file)) {
			char full_msg[0x400];
			va_list args2;
			va_start(args2, format);
			vsnprintf(full_msg, sizeof(full_msg), format, args2);
			va_end(args2);

			char log_entry[0x600];
			if (verbose < 2) {
				snprintf(log_entry, sizeof(log_entry), "%s%s%s\n", file_msg, full_msg, "");
			} else {
				snprintf(log_entry, sizeof(log_entry), "%s%s\n---\n", file_msg, full_msg);
			}
			log_to_file(log_entry);
		}
	}
	pthread_mutex_unlock(&log_mutex);
	return ret;
}
