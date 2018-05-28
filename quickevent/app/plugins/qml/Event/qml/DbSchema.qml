import QtQml 2.0
import qf.core 1.0
import qf.core.sql.def 1.0

Schema {
	//name: 'main'
	tables: [
		Table { id: enumz; name: 'enumz'
			fields: [
				Field { name: 'id'; type: Serial { primaryKey: true } },
				Field { name: 'groupName'; type: String {} },
				Field { name: 'groupId'; type: String {} },
				Field { name: 'pos'; type: Int {} },
				Field { name: 'caption'; type: String {} },
				Field { name: 'color'; type: String {} },
				Field { name: 'value'; type: String {} }
			]
			indexes: [
				Index {fields: ['groupName' ,'groupId']; unique: true }
			]
		},
		Table { id: config; name: 'config'
			fields: [
				Field { name: 'ckey'; type: String {} },
				Field { name: 'cname'; type: String {} },
				Field { name: 'cvalue'; type: String {} },
				Field { name: 'ctype'; type: String {} }
			]
			indexes: [
				Index { fields: ['ckey']; primary: true }
			]
		},
		Table { name: 'stages'
			fields: [
				Field { name: 'id'; type: Int {} },
				Field { name: 'startDateTime'; type: DateTime {} },
				Field { name: 'useAllMaps'
					type: Boolean {}
					defaultValue: false
					notNull: true
				},
				Field { name: 'drawingConfig'; type: String {} }
			]
			indexes: [
				Index {fields: ['id']; primary: true }
			]
		},
		Table { name: 'courses'
			fields: [
				Field { name: 'id'; type: Serial { primaryKey: true } },
				//Field { name: 'stageId'; type: Int { } },
				Field { name: 'name'; type: String { } },
				Field { name: 'length'; type: Int { } },
				Field { name: 'climb'; type: Int { } },
				Field { name: 'note'; type: String { } }
			]
		},
		Table { name: 'codes'
			fields: [
				Field { name: 'id'; type: Serial { primaryKey: true } },
				Field { name: 'code'; type: Int { } },
				Field { name: 'altCode'; type: Int { } },
				Field { name: 'outOfOrder'; 
					type: Boolean { } 
					defaultValue: false;
					notNull: true
				},
				Field { name: 'radio'; 
					type: Boolean { } 
					defaultValue: false
					notNull: true
				},
				Field { name: 'note'; type: String { } }
			]
			indexes: [
				//Index {fields: ['stageId', 'code']; unique: true }
			]
		},
		Table { name: 'coursecodes'
			fields: [
				Field { name: 'id'; type: Serial { primaryKey: true } },
				Field { name: 'courseId'; type: Int { } },
				Field { name: 'position'; type: Int { } },
				Field { name: 'codeId'; type: Int { } }
			]
			indexes: [
				Index {fields: ['courseId']; references: ForeignKeyReference {table: 'courses'; fields: ['id']; } },
				Index {fields: ['codeId']; references: ForeignKeyReference {table: 'codes'; fields: ['id']; } },
				Index {
					fields: ['courseId', 'position']
					unique: false
				}
			]
		},
		Table { name: 'classes'
			fields: [
				Field { name: 'id'; type: Serial { primaryKey: true } },
				Field { name: 'name'; type: String { } }
			]
			indexes: [
				Index { fields: ['name']; unique: true }
			]
		},
		Table { name: 'classdefs'
			fields: [
				Field { name: 'id'; type: Serial { primaryKey: true } },
				Field { name: 'classId'; type: Int { } },
				Field { name: 'stageId'; type: Int { } },
				Field { name: 'courseId'; type: Int { } },
				Field { name: 'legCount'; type: Int { } },
				Field { name: 'startSlotIndex'; type: Int { }
					defaultValue: -1;
					notNull: true
	 			},
				Field { name: 'startTimeMin'; type: Int { } },
				Field { name: 'startIntervalMin'; type: Int { } },
				Field { name: 'vacantsBefore'; type: Int { } 
						comment: 'place n vacants gap before first competitor in class start list'
				},
				Field { name: 'vacantEvery'; type: Int { } 
					comment: 'place vacant every n-th competitor in class start list'
				},
				Field { name: 'vacantsAfter'; type: Int { } 
					comment: 'place n vacants gap after last competitor in class start list'
				},
				Field { name: 'mapCount'; type: Int { } },
				Field { name: 'resultsCount'; type: Int { }
					comment: 'number of finished competitors, when the results were printed'
				},
				Field { name: 'resultsPrintTS'; type: DateTime { }
					comment: 'when results for this class were printed last time'
				},
				Field { name: 'lastStartTimeMin'; type: Int { } },
				Field { name: 'drawLock'; type: Boolean { }
					defaultValue: false
					notNull: true
					comment: 'The draw of this class is prohibited'
				}
			]
			indexes: [
				Index {fields: ['stageId']; references: ForeignKeyReference {table: 'stages'; fields: ['id']; } },
				Index {fields: ['classId']; references: ForeignKeyReference {table: 'classes'; fields: ['id']; } },
				Index {fields: ['courseId']; references: ForeignKeyReference {table: 'courses'; fields: ['id']; } }
			]
		},
		Table { name: 'competitors'
			fields: [
				Field { name: 'id'; type: Serial { primaryKey: true } },
				Field { name: 'startNumber'; type: Int {} },
				Field { name: 'classId'; type: Int {} },
				//Field { name: 'courseId'; type: String { length: 10 } },
				Field { name: 'firstName'; type: String {} },
				Field { name: 'lastName'; type: String {} },
				Field { name: 'registration'; type: String { length: 10 } },
				Field { name: 'licence'; type: String { length: 1 } },
				Field { name: 'club'; type: String { } },
				Field { name: 'country'; type: String { } },
				Field { name: 'siId'; type: Int { } },
				Field { name: 'ranking'; type: Int { } },
				Field { name: 'note'; type: String { } },
				Field { name: 'importId'; type: Int {} }
			]
			indexes: [
				Index {fields: ['classId']; references: ForeignKeyReference {table: 'classes'; fields: ['id']; } },
				Index {fields: ['importId'] }
			]
		},
		Table { name: 'runs'
			fields: [
				Field { name: 'id'; type: Serial { primaryKey: true } },
				Field { name: 'competitorId'; type: Int {} },
				Field { name: 'siId'; type: Int {} },
				Field { name: 'stageId'; type: Int {}
					defaultValue: 1;
					notNull: true
				},
				Field { name: 'leg'; type: Int {}
					//defaultValue: 0;
					//notNull: true
				},
				Field { name: 'relayId'; type: Int {} },
				Field { name: 'startTimeMs'; type: Int {}
					comment: 'in miliseconds'
				},
				Field { name: 'finishTimeMs'; type: Int {}
					comment: 'in miliseconds'
				},
				Field { name: 'timeMs'; type: Int {}
					comment: 'in miliseconds since event run'
				},
				Field { name: 'isRunning'; type: Boolean { }
					defaultValue: true;
					comment: "Competitor is running in this stage"
				},
				Field { name: 'notCompeting'; type: Boolean { }
					defaultValue: false;
					notNull: true
					comment: "Competitor does run in this stage but not competing"
				},
				Field { name: 'disqualified'; type: Boolean { }
					defaultValue: false;
					notNull: true
				},
				Field { name: 'misPunch'; type: Boolean { }
					defaultValue: false;
					notNull: true
				},
				Field { name: 'badCheck'; type: Boolean { }
					defaultValue: false;
					notNull: true
				},
				Field { name: 'cardLent'; type: Boolean { }
					defaultValue: false;
					notNull: true
				},
				Field { name: 'cardReturned'; type: Boolean { }
					defaultValue: false;
					notNull: true
				},
				Field { name: 'importId'; type: Int {} }
			]
			indexes: [
				Index {
					fields: ['competitorId'];
					references: ForeignKeyReference {
						table: 'competitors';
						fields: ['id'];
						onUpdate: 'RESTRICT';
						onDelete: 'RESTRICT';
					}
				},
				Index {fields: ['stageId']; references: ForeignKeyReference {table: 'stages'; fields: ['id']; } },
				Index {fields: ['stageId', 'leg', 'competitorId']; unique: true },
				Index {fields: ['stageId', 'leg', 'siId', 'isRunning']; unique: false }
			]
		},
		Table { name: 'relays'
			fields: [
				Field { name: 'id'; type: Serial { primaryKey: true } },
				Field { name: 'number'; type: Int {} },
				Field { name: 'classId'; type: Int {} },
				Field { name: 'name'; type: String {} },
				//Field { name: 'club'; type: String {} },
				Field { name: 'note'; type: String {} },
				Field { name: 'importId'; type: Int {} }
			]
			indexes: [
				Index {fields: ['classId']; references: ForeignKeyReference {table: 'classes'; fields: ['id']; } },
				Index {fields: ['name'] },
				Index {fields: ['number'] }
			]
		},
		Table { name: 'runlaps'
			fields: [
				Field { name: 'id'; type: Serial { primaryKey: true } },
				Field { name: 'runId'; type: Int {} },
				Field { name: 'position'; type: Int {} },
				Field { name: 'code'; type: Int {} },
				Field { name: 'stpTimeMs'; type: Int { } },
				Field { name: 'lapTimeMs'; type: Int { } }
			]
			indexes: [
				Index {fields: ['runId', 'position'] },
				Index {fields: ['position', 'stpTimeMs'] },
				Index {fields: ['position', 'lapTimeMs'] }
			]
		},
		Table { name: 'clubs'
			fields: [
				Field { name: 'id'; type: Serial { primaryKey: true } },
				Field { name: 'name'; type: String {} },
				Field { name: 'abbr'; type: String {} },
				Field { name: 'importId'; type: Int { } }
			]
			indexes: [
				Index {fields: ['abbr'] }
			]
		},
		Table { name: 'registrations'
			fields: [
				Field { name: 'id'; type: Serial { primaryKey: true } },
				Field { name: 'firstName'; type: String {} },
				Field { name: 'lastName'; type: String {} },
				Field { name: 'registration'; type: String { length: 10 } },
				Field { name: 'licence'; type: String { length: 1 } },
				Field { name: 'clubAbbr'; type: String { } },
				Field { name: 'country'; type: String { } },
				Field { name: 'siId'; type: Int { } },
				//Field { name: 'nameSearchKey'; type: String {} },
				Field { name: 'importId'; type: Int { } }
			]
			indexes: [
				Index {fields: ['registration'] }
				//Index {fields: ['nameSearchKey'] }
			]
		},
		Table { name: 'cards'
			fields: [
				Field {
					name: 'id'
					type: Serial { primaryKey: true }
				},
				Field { name: 'runId'
					type: Int { }
				},
				Field { name: 'runIdAssignTS'
					type: DateTime { }
				},
				Field { name: 'stageId'
					type: Int { }
					comment: 'We cannot take stageId from runId linked table, because we need select cards for stage even without runId assigned'
				},
				Field { name: 'stationNumber'
					type: Int {
						length: 3
						unsigned: true
					}
					//notNull: true
					defaultValue: 0
				},
				Field { name: 'siId'
					type: Int {
						unsigned: true
					}
				},
				Field { name: 'checkTime'
					type: Int {
						unsigned: true
					}
					comment: 'seconds in range 0 - 12 hours'
				},
				Field { name: 'startTime'
					type: Int {
						unsigned: true
					}
					comment: 'seconds in range 0 - 12 hours'
				},
				Field { name: 'finishTime'
					type: Int {
						unsigned: true
					}
					comment: 'seconds in range 0 - 12 hours'
				},
				Field { name: 'punches'
					type: String {
						length: 65536
						charset: 'latin1'
					}
					comment: 'JSON of format [[code, time, msec, day_of_week, week_cnt], ...]}'
				},
				Field { name: 'readerConnectionId'
					type: Int { }
					comment: 'connection id of QuickEvent instance which has read this card'
				},
				Field { name: 'printerConnectionId'
					type: Int { }
					comment: 'connection id of QuickEvent instance which has printed this strip'
				}
  			]
			indexes: [
				Index { fields: ['readerConnectionId']; unique: false },
				Index { fields: ['printerConnectionId']; unique: false },
				Index { fields: ['stageId', 'siId']; unique: false },
				Index { fields: ['runId']; unique: false }
			]
		},
		Table { name: 'punches'
			fields: [
				Field { name: 'id'; type: Serial { primaryKey: true } },
				Field { name: 'code'; type: Int { } },
				Field { name: 'siId'; type: Int {} },
				Field { name: 'time'; type: Int {}
					comment: 'seconds in range 0 - 12 hours'
				},
				Field { name: 'msec'; type: Int {}
					comment: 'msec part od punch time'
				},
				Field { name: 'stageId'; type: Int { }
					comment: 'We cannot take stageId from runId linked table, because we need select punches for stage even without runId assigned'
				},
				Field { name: 'runId'; type: Int {} },
				Field { name: 'timeMs'; type: Int {}
					comment: 'in miliseconds since event start'
				},
				Field { name: 'runTimeMs'; type: Int {}
					comment: 'in miliseconds since runner event start'
				},
				Field { name: 'marking'; type: String {}
					notNull: true
					defaultValue: 'race';
					comment: 'possible values: race | entries'
				}
			]
			indexes: [
				Index {fields: ['marking', 'stageId', 'code']; unique: false },
				Index {fields: ['runId']; unique: false }
			]
		},
		Table { name: 'lentcards'
			fields: [
				Field { name: 'siId'; type: Int { primaryKey: true } },
				Field { name: 'ignored'; type: Boolean { }
					notNull: true
					defaultValue: false
				},
				Field { name: 'note'; type: String { } }
			]
		}
	]
	inserts: [
		/*
		Insert {
			table: enumz
			fields: ['groupName', 'groupId', 'pos', 'caption']
			rows: [
				['cardReader.punchMarking', 'race', 1, qsTr('Race')],
				['cardReader.punchMarking', 'entries', 2, qsTr('Entries')]
			]
		},
		*/
		Insert {
			table: config
			rows: [
				['db.version', qsTr('Data version'), '{{minDbVersion}}', 'int']
				/*
				['event.stageCount', qsTr('Stage count'), '0', 'int'],
				['event.name', qsTr('Event name'), '', 'QString'],
				['event.date', qsTr('Event date'), '', 'QDate'],
				['event.description', qsTr('Event description'), '', 'QString'],
				['event.place', qsTr('Event place'), '', 'QString'],
				['event.mainReferee', qsTr('Main referee'), '', 'QString'],
				['event.director', qsTr('Director'), '', 'QString'],
				['event.importId', qsTr('Import ID'), '', 'int']
				*/
			]
		}
	]
	Component.onCompleted: {
		Log.info("DbSchema created");
	}

}
