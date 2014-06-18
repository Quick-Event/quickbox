import qf.core
import qf.core.sql.def

Schema {
	name: 'untitled'
	tables: [
		Table {
			name: 'cards'
			fields: [
				Field {
					name: 'id'
					type: Serial {}
				},
				Field{
					name: 'stationNumber'
					type: Int {
						length: 3
						unsigned: true
					}
					notNull: true
					defaultValue: 0
				},
				Field{
					name: 'idsi'
					type: Int {
						unsigned: true
					}
					notNull: true
				},
				Field{
					name: 'checkTime'
					type: Int {
						unsigned: true
					}
					notNull: true
					comment: 'seconds in range 0 - 12 hours'
				},
				Field{
					name: 'startTime'
					type: Int {
						unsigned: true
					}
					notNull: true
					comment: 'seconds in range 0 - 12 hours'
				},
				Field{
					name: 'finishTime'
					type: Int {
						unsigned: true
					}
					notNull: true
					comment: 'seconds in range 0 - 12 hours'
				},
				Field{
					name: 'punches'
					type: String {
						length: 65536
						charset: 'latin1'
					}
					notNull: true
					comment: 'JSON of format {codes: [...], times: [...], count: n}'
				}
  			]
			indicies: [
				Index {
					fields: ['id']
					primary: true
				}
			]
		}
	]
}