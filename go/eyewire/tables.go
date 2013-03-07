package main

import (
	"github.com/coopernurse/gorp"
	"time"
)

func SetupTables(dbm *gorp.DbMap) {
	dbm.AddTableWithName(Account{}, "accounts").SetKeys(true, "id")
	dbm.AddTableWithName(Task{}, "tasks").SetKeys(true, "id")
	dbm.AddTableWithName(Validation{}, "validations").SetKeys(true, "id")
	dbm.AddTableWithName(Tutorial{}, "tutorials").SetKeys(true, "id")
}

type Account struct {
	Id          int64     `db:"id"`
	Username    *string   `db:"username"`
	Password    *string   `db:"password"`
	Created     time.Time `db:"created"`
	Email       string    `db:"email"`
	Token       *string   `db:"token"`
	Trained     int8      `db:"trained"`
	Class       *int64    `db:"class"`
	Timeout     time.Time `db:"timeout"`
	Email_prefs *string   `db:"email_prefs"`
	Fbid        *string   `db:"fbid"`
	Prefs       *string   `db:"prefs"`
	Identifier  *string   `db:"identifier"`
	Accepted    *int8     `db:"accepted"`
	Country     *string   `db:"country"`
	Weight      *float32  `db:"weight"`
}

type Task struct {
	Id              int64     `db:"id"`
	Seeds           string    `db:"seeds"`
	Status          int64     `db:"status"`
	Created         time.Time `db:"created"`
	Difficulty      int64     `db:"difficulty"`
	Instructions    *string   `db:"instructions"`
	Task_seq        int64     `db:"task_seq"`
	Cell            *int64    `db:"cell"`
	Channel_id      *int64    `db:"channel_id"`
	Segmentation_id *int64    `db:"segmentation_id"`
	Root_id         *int64    `db:"root_id"`
	Depth           *int64    `db:"depth"`
	Right_edge      *int64    `db:"right_edge"`
	Left_edge       *int64    `db:"left_edge"`
	Confidence      *float32  `db:"confidence"`
	Underconfidence *float32  `db:"underconfidence"`
	Enabled         *int8     `db:"enabled"`
}

type Validation struct {
	Id          int64      `db:"id"`
	Task_id     int64      `db:"task_id"`
	User_id     int64      `db:"user_id"`
	Status      int64      `db:"status"`
	Segments    *string    `db:"segments"`
	Finish      *time.Time `db:"finish"`
	Duration    int64      `db:"duration"`
	Note        *string    `db:"note"`
	Orientation *int32     `db:"orientation"`
	Weight      *float32   `db:"weight"`
	Inspected   *int8      `db:"inspected"`
	Scored      *int8      `db:"scored"`
	Score       *int64     `db:"score"`
	Tb          *int8      `db:"tb"`
	Tp          *int64     `db:"tp"`
	Fp          *int64     `db:"fp"`
	Fn          *int64     `db:"fn"`
}

type Tutorial struct {
	Id           int64   `db:"id"`
	Task_id      *int64  `db:"task_id"`
	Difficulty   *int32  `db:"difficulty"`
	Instructions *string `db:"instructions"`
	Sequence     *int32  `db:"sequence"`
	Celltype     *string `db:"celltype"`
}
