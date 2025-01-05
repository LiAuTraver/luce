plugins {
	`cpp-application`
	`cpp-unit-test`
}

group = "accat"
version = "1.0.0"
application {
	targetMachines.add(machines.windows.x86_64)
}