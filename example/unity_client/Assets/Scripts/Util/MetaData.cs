using UnityEngine;
using System;
using System.Collections.Generic;
using System.Reflection;
using System.Linq;
using System.Collections;

public class MetaData
{
	public class Header
	{
		public int index;
		public string name;
		public Header child;
	}

	public class Cell
	{
		public Header header;
		public string value;
	}

	public class Row
	{
		public int row_num;
		public List<Cell> cells = new List<Cell>();
	}

	public void Init(Row row)
	{
		Type type = this.GetType();
		IEnumerable<FieldInfo> fieldInfos = type.GetFields().Select(field => field);

		Dictionary<string, FieldInfo> members = new Dictionary<string, FieldInfo>();
		foreach (FieldInfo fieldInfo in fieldInfos)
		{
			Debug.Log("\nName    : " + fieldInfo.Name);
			Debug.Log("FieldType : " + fieldInfo.FieldType);

			members.Add(fieldInfo.Name, fieldInfo);
		}

		foreach (Cell cell in row.cells)
		{
			Header header = cell.header;
			if ("" == cell.value)
			{
				continue;
			}

			if (false == members.ContainsKey(header.name))
			{
				continue;
			}

			FieldInfo fieldInfo = members[header.name];
			if (typeof(System.Int32) == fieldInfo.FieldType)
			{
				fieldInfo.SetValue(this, Int32.Parse(cell.value));
				Debug.Log("Value : " + fieldInfo.GetValue(this));
			}
			else if (typeof(string) == fieldInfo.FieldType)
			{
				fieldInfo.SetValue(this, cell.value);
			}
			// https://stackoverflow.com/questions/27771648/determine-if-a-type-is-a-generic-list-of-enum-types
			else if (typeof(IEnumerable) == fieldInfo.FieldType)
			{
				Debug.Log("enumberable");
			}
		}
	}
	protected void Bind(string name, ref bool member)
	{
		bind_functions.Add(name, (object obj) => {
			FieldInfo fld = typeof(MetaData).GetField(name);
			fld.GetValue(null);
		});
	}

	private Dictionary<string, Action<object>> bind_functions = new Dictionary<string, Action<object>>();

	public class Reader<T> where T : MetaData, new()
	{
		private List<T> meta_datas = null;

		public void Read(CSVReader reader)
		{
			meta_datas = new List<T>();

			List<Header> headers = new List<Header>();
			foreach (string columnName in reader.GetColumnNames())
			{
				headers.Add(ReadColumnName(columnName));
			}

			int rowNum = 1;
			foreach (CSVReader.Row row in reader)
			{
				Row root = new Row();
				root.row_num = rowNum++;
				for (int i = 0; i < reader.GetColumnNames().Count; i++)
				{
					Cell cell = new Cell();
					cell.header = headers[i];
					cell.value = row.GetValue(i);
					root.cells.Add(cell);
				}
				T meta = new T();
				meta.Init(root);
			}
		}

		private Header ReadColumnName(string columnName)
		{
			Header header = new Header();
			string cellValue = columnName.ToLower();
			string column = cellValue;
			int dotPos = cellValue.IndexOf('.');
			if (-1 != dotPos)
			{
				column = cellValue.Substring(0, dotPos);
			}
			else
			{
				dotPos = Int32.MaxValue;
			}

			int braceStartPos = column.IndexOf('[');
			int braceEndPos = column.IndexOf(']');
			if (-1 != braceStartPos && -1 != braceEndPos)
			{
				string strIndex = column.Substring(braceStartPos + 1, braceEndPos - braceStartPos - 1);
				header.index = Int32.Parse(strIndex);
			}
			if (-1 == braceStartPos)
			{
				braceStartPos = Int32.MaxValue; 
			}
			if (-1 == braceEndPos)
			{
				braceEndPos = Int32.MaxValue;
			}

			int columnEndPos = Math.Min(dotPos, braceStartPos);
			columnEndPos = Math.Min(columnEndPos, column.Length);
			header.name = column.Substring(0, columnEndPos);
			Debug.Log("colunm name:" + header.name);
			if (Int32.MaxValue != dotPos)
			{
				header.child = ReadColumnName(cellValue.Substring(dotPos + 1));
			}
			return header;
		}

	}
}