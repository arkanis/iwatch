#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <sys/wait.h>
#include <sys/types.h>


int main(int argc, char **argv){
	
	// Handle command line arguments
	if (argc < 3){
		fprintf(stderr, "iwatch v0.4 - execute a command as soon as a watched file changes.\n");
		fprintf(stderr, "Usage: %s file... command\n", argv[0]);
		return -1;
	}
	
	char*  command    = argv[argc - 1];
	char** files      = argv + 1;
	size_t file_count = argc - 2;
	int    watch_ids[file_count];
	
	// Setup inotify and watch each file
	int event_queue = inotify_init();
	if (event_queue == -1){
		perror("inotify_init");
		return -1;
	}
	
	for(size_t i = 0; i < file_count; i++){
		int watch_id = inotify_add_watch(event_queue, files[i], IN_CREATE | IN_CLOSE_WRITE);
		watch_ids[i] = watch_id;
		if (watch_id == -1)
			fprintf(stderr, "Can't watch %s: %s\n", files[i], strerror(errno));
	}
	
	printf("Watching for changes, use ctrl + c to exit.\n");
	
	// Read incoming events
	char buffer[4096];
	struct inotify_event *event = (void*)buffer;
	while(true){
		int bytes_read = 0;
		do {
			bytes_read = read(event_queue, event, sizeof(buffer));
		} while(bytes_read == -1 && errno == EINTR);
		if (bytes_read == -1){
			perror("Read of event structure failed");
			break;
		}
		
		// Display a message with the changed file
		size_t i;
		for(i = 0; i < file_count; i++){
			if (watch_ids[i] == event->wd)
				break;
		}
		char* file = (i < file_count) ? files[i] : "unknown file";
		
		if (event->mask & IN_CREATE)
			printf("File created in %s, executing: %s\n", file, command);
		else if (event->mask & IN_CLOSE_WRITE)
			printf("File %s written to, executing: %s\n", file, command);
		
		// Execute command
		if (event->mask & IN_CREATE || event->mask & IN_CLOSE_WRITE){
			pid_t child = fork();
			if (child == 0) {
				execl("/bin/sh", "/bin/sh", "-c", command, NULL);
			} else {
				waitpid(child, NULL, 0);
			}
		}
	}
	
	close(event_queue);
	
	return 0;
}