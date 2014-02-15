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
	
	if (argc < 4 || strcmp(argv[2], "--") != 0){
		fprintf(stderr, "usage: %s target-file -- command\n", argv[0]);
		return -1;
	}
	
	char *target_file = argv[1];
	char *action = argv[3];
	
	int event_queue = inotify_init();
	if (event_queue == -1){
		perror("inotify_init");
		return -1;
	}
	
	int watch_id = inotify_add_watch(event_queue, target_file, IN_CREATE | IN_CLOSE_WRITE);
	if (watch_id == -1){
		perror("inotify_add_watch");
		return -1;
	}
	
	char buffer[4096];
	struct inotify_event *event = (void*)buffer;
	while(true){
		int bytes_read = 0;
		do {
			bytes_read = read(event_queue, event, sizeof(buffer));
		} while(bytes_read == -1 && errno == EINTR);
		if (bytes_read == -1){
			perror("read of event structure failed");
			break;
		}
		
		if (event->mask & IN_CREATE){
			printf("file created in %s, executing: %s\n", target_file, action);
			pid_t child = fork();
			if (child == 0) {
				execl("/bin/sh", "/bin/sh", "-c", action, NULL);
			} else {
				waitpid(child, NULL, 0);
			}
		}

		if (event->mask & IN_CLOSE_WRITE){
			printf("file %s written to, executing: %s\n", target_file, action);
			pid_t child = fork();
			if (child == 0) {
				execl("/bin/sh", "/bin/sh", "-c", action, NULL);
			} else {
				waitpid(child, NULL, 0);
			}
		}
		
		if (event->mask & IN_DELETE_SELF) {
			printf("%s deleted, nothing more to watch. Have a nice day.\n", target_file);
			break;
		}
	}
	
	close(event_queue);
	
	return 0;
}