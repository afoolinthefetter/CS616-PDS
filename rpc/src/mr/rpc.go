package mr

import (
	"os"
	"strconv"
)

type KeyValue struct {
	Key   string
	Value string
}

type ByKey []KeyValue

func (a ByKey) Len() int {
	return len(a)
}
func (a ByKey) Swap(i, j int) {
	a[i], a[j] = a[j], a[i]
}
func (a ByKey) Less(i, j int) bool {
	return a[i].Key < a[j].Key
}

type InitWorkerArgs struct {
}

type InitWorkerReply struct {
	NMap    int
	NReduce int
}

type GetTaskArgs struct {
}

type GetTaskReply struct {
	Scheduled bool
	Phase     string
	TaskId    int
	Filename  string
	Content   string
}

type CommitTaskArgs struct {
	Phase  string
	TaskId int
}

type CommitTaskReply struct {
	Done bool
}

func coordinatorSock() string {
	s := "/var/tmp/cs612-mr-"
	s += strconv.Itoa(os.Getuid())
	return s
}
