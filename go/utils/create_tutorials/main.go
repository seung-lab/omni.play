package main

import (
	"database/sql"
	"encoding/json"
	"flag"
	"github.com/coopernurse/gorp"
	_ "github.com/ziutek/mymysql/godrv"
	"log"
)

var host = flag.String("host", "localhost", "The host to connect to.")
var database = flag.String("database", "", "The DB to connect to.")
var user = flag.String("user", "", "The user to connect as.")
var password = flag.String("pass", "", "The password to use.")
var sequence = flag.Int64("seq", 0, "The first sequence number to use.")
var celltype = flag.String("celltype", "", "The celltype of the tutorial.")

func getConnString() string {
	flag.Parse()

	var connString string
	if host != nil && *host != "localhost" {
		connString += "tcp:" + *host + "*"
	}
	if database != nil && *database != "" {
		connString += *database + "/" + *user + "/" + *password
	}
	return connString
}

func getTasksForCell(dbm *gorp.DbMap, cell int) []interface{} {
	obj, err := dbm.Select(Task{}, "SELECT * FROM tasks WHERE cell = ? AND status = 0", cell)
	if err != nil {
		log.Fatal(err.Error())
	}
	return obj
}

func thresholdSegments(segs string) string {
	var segments map[string]interface{}
	json.Unmarshal([]byte(segs), &segments)
	for seg, conf := range segments {
		if confidence, ok := conf.(float64); !ok {
			log.Println("Bad Segments List", segs)
			delete(segments, seg)
		} else if confidence <= .2 {
			delete(segments, seg)
		} else {
			segments[seg] = 1
		}
	}

	bytes, _ := json.Marshal(segments)
	return string(bytes)
}

func main() {
	connString := getConnString()

	db, err := sql.Open("mymysql", connString)
	if err != nil {
		log.Fatal("Unable to connect to server: " + err.Error())
	}
	defer db.Close()

	log.Println("Connected to", connString)

	dbm := &gorp.DbMap{Db: db, Dialect: gorp.MySQLDialect{"InnoDB", "UTF8"}}
	// dbm.TraceOn("[gorp]", log.New(os.Stdout, "mysql-util:", log.Lmicroseconds))
	SetupTables(dbm)

	tasks := getTasksForCell(dbm, 10)

	difficulty := int32(-1)
	instructions := ""
	seq := int32(*sequence)

	for _, t := range tasks {
		task, ok := t.(*Task)
		if !ok {
			log.Println(err.Error())
			continue
		}

		val, err := dbm.Select(Validation{}, "SELECT * FROM validations WHERE task_id = ? AND status = 9", task.Id)
		if err != nil {
			log.Println(err.Error())
			continue
		}
		if len(val) != 1 {
			log.Println("Didn't find the right number of Validations.")
			continue
		}

		task.Status = 3
		task.Difficulty = 0
		task.Instructions = nil
		task.Task_seq = 0
		task.Cell = nil
		task.Root_id = nil
		task.Depth = nil
		task.Right_edge = nil
		task.Left_edge = nil
		task.Confidence = nil
		task.Underconfidence = nil
		task.Enabled = nil
		err = dbm.Insert(task)
		if err != nil {
			log.Println("Failed to insert Task:", task.Id, err.Error())
			continue
		}

		validation := val[0].(*Validation)
		*validation.Segments = thresholdSegments(*validation.Segments)

		validation.Task_id = task.Id
		*validation.Weight = 1
		validation.User_id = 0
		validation.Finish = nil
		validation.Duration = 0
		validation.Note = nil
		validation.Orientation = nil
		validation.Weight = nil
		validation.Inspected = nil
		validation.Scored = nil
		validation.Score = nil
		validation.Tb = nil
		validation.Tp = nil
		validation.Fp = nil
		validation.Fn = nil
		err = dbm.Insert(validation)
		if err != nil {
			log.Println("Failed to insert Validation:", task.Id, err.Error())
			continue
		}

		t := &Tutorial{
			Task_id:      &task.Id,
			Difficulty:   &difficulty,
			Instructions: &instructions,
			Sequence:     &seq,
			Celltype:     celltype,
		}
		err = dbm.Insert(t)
		if err != nil {
			log.Println("Failed to insert Tutorial:", task.Id, err.Error())
			continue
		}

		*sequence += 1
	}
}
