package mr

import (
	"encoding/json"
	"fmt"
	"hash/fnv"
	"log"
	"net/rpc"
	"os"
	"sort"
	"time"
)



func ihash(key string) int {
	h := fnv.New32a()
	h.Write([]byte(key))
	return int(h.Sum32() & 0x7fffffff)
}

func Worker(
	mapf func(string, string) []KeyValue,
	reducef func(string, []string) string,
) {
	initWorkerArgs := InitWorkerArgs{}
	initWorkerReply := InitWorkerReply{}
	call("Coordinator.InitWorker", &initWorkerArgs, &initWorkerReply)
	nMap := initWorkerReply.NMap
	nReduce := initWorkerReply.NReduce

	for {
		getTaskArgs := GetTaskArgs{}
		getTaskReply := GetTaskReply{}
		call("Coordinator.GetTask",&getTaskArgs,&getTaskReply,)
		if !getTaskReply.Scheduled {
			time.Sleep(time.Second*2)
			continue
		}

		//extract the reply
		taskId := getTaskReply.TaskId
		phase := getTaskReply.Phase
		filename := getTaskReply.Filename
		content := getTaskReply.Content

		if phase == "map" {
			mapResult := mapf(filename, content)
			fmt.Println("Map Length:", len(mapResult), ", Key:", mapResult[0].Key, ", Value:", mapResult[0].Value)
			outputFileList := []*os.File{}
			for reduceId := 0; reduceId < nReduce; reduceId++ {
				resultFilename := fmt.Sprintf("mr-%d-%d",taskId,reduceId)
				tempFile, err := os.Create(resultFilename)
				if err != nil {
					break
				}
				outputFileList = append(outputFileList, tempFile)
			}

			for _, kv := range mapResult {
				ihashV := ihash(kv.Key)
				//fmt.Println("some ihashV: ",ihashV)
				reduceId := ihashV % nReduce
				outputFile := outputFileList[reduceId]

				enc := json.NewEncoder(outputFile)
				encodeErr := enc.Encode(&kv)
				if encodeErr != nil {
					return
				}
			}

			for reduceId := 0; reduceId < nReduce; reduceId++ {
				resultFilename := fmt.Sprintf("mr-%d-%d",taskId,reduceId)
				outputFile := outputFileList[reduceId]
				os.Rename(outputFile.Name(), resultFilename)
				outputFile.Close()
			}
		}

		if phase == "reduce" {
			reduceInput := []KeyValue{}
			for mapId := 0; mapId < nMap; mapId++ {
				inputFilename := fmt.Sprintf("mr-%d-%d",mapId,taskId)

				inputFile, openErr := os.Open(inputFilename)
				if openErr != nil {
					break
				}

				dec := json.NewDecoder(inputFile)
				for {
					var kv KeyValue
					if decodeErr := dec.Decode(&kv); decodeErr != nil {
						break
					}
					reduceInput = append(reduceInput, kv)
				}
			}

			sort.Sort(ByKey(reduceInput))

			outputFileName := fmt.Sprintf("mr-out-%d",taskId)

			outputFile, err := os.Create(outputFileName)
			if err != nil {
			    break
			}
			i := 0
			for i < len(reduceInput) {
				j := i + 1
				for j < len(reduceInput) && reduceInput[j].Key == reduceInput[i].Key {
					j++
				}
				values := []string{}
				for k := i; k < j; k++ {
					values = append(values, reduceInput[k].Value)
				}
				output := reducef(reduceInput[i].Key, values)

				fmt.Fprintf(outputFile, "%v %v\n", reduceInput[i].Key, output)

				i = j
			}

			os.Rename(outputFile.Name(), outputFileName)
			outputFile.Close()
		}

		CommitTaskArgs := CommitTaskArgs{
			Phase:  phase,
			TaskId: taskId,
		}
		CommitTaskReply := CommitTaskReply{}
		call("Coordinator.CommitTask", &CommitTaskArgs, &CommitTaskReply)
		if CommitTaskReply.Done {
			return
		}
	}
}

func call(rpcname string, args interface{}, reply interface{}) bool {
	sockname := coordinatorSock()
	c, err := rpc.DialHTTP("unix", sockname)
	if err != nil {
		log.Fatal("dialing:", err)
	}
	defer c.Close()

	err = c.Call(rpcname, args, reply)
	if err == nil {
		return true
	}

	fmt.Println(err)
	return false
}
