#include <signal.h>
#include <unistd.h>

#include "mdns.h"
#include "urtc.h"


int main() {
	sigset_t ss;
	int signal;

	const char *stun[] = {
		"stun.l.google.com:19302",
		"stun2.l.google.com:19302",
		NULL
	};

	sigemptyset(&ss);
	sigaddset(&ss, SIGINT);
	sigaddset(&ss, SIGTERM);
	sigaddset(&ss, SIGQUIT);

	urtc_peerconn_t *pc = urtc_peerconn_create(stun);

	// query for raspberry.local
	mdns_query("raspberry");

	sigwait(&ss, &signal);

	urtc_peerconn_destroy(pc);

	return 0;
}
