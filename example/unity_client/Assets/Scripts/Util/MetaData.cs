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

	public MetaData()
	{
	}

	public void Init(Row row)
	{
		foreach (Cell cell in row.cells)
		{
			Header header = cell.header;
			if ("" == cell.value)
			{
				continue;
			}

			Type type = this.GetType();
			FieldInfo fieldInfo = type.GetField(cell.header.name);
			if (null == fieldInfo)
			{
				continue;
			}

			object member = fieldInfo.GetValue(this);
			SetValue(fieldInfo.FieldType, ref member, cell);
			fieldInfo.SetValue(this, member);
		}
	}
	public virtual void OnLoad() { }
	protected void CustomBind(string name, ValueTransform translator)
	{
		bind_functions.Add(name, translator);
	}
	public void SetValue(Type type, ref object member, Cell cell)
	{
		Header header = cell.header;
		if (true == bind_functions.ContainsKey(header.name))
		{
			bind_functions[header.name](ref member, cell.value);
			return;
		}

		if (type == typeof(Boolean))
		{
			member = Boolean.Parse(cell.value);
		}
		else if (type == typeof(Int16))
		{
			member = Int16.Parse(cell.value);
		}
		else if (type == typeof(UInt16))
		{
			member = UInt16.Parse(cell.value);
		}
		else if (type == typeof(Int32))
		{
			member = Int32.Parse(cell.value);
		}
		else if (type == typeof(UInt32))
		{
			member = UInt32.Parse(cell.value);
		}
		else if (type == typeof(Int64))
		{
			member = Int64.Parse(cell.value);
		}
		else if (type == typeof(UInt64))
		{
			member = UInt64.Parse(cell.value);
		}
		else if (type == typeof(float))
		{
			member = float.Parse(cell.value);
		}
		else if (type == typeof(double))
		{
			member = double.Parse(cell.value);
		}
		else if (type == typeof(string))
		{
			member = cell.value;
		}
		else if (type == typeof(DateTime))
		{
			member = DateTime.Parse(cell.value);
		}
		else if (true == typeof(IList).IsAssignableFrom(type))
		{
			if (null == member)
			{
				member = (IList)Activator.CreateInstance(type);
			}

			IList list = (IList)member;
			Type itemType = type.GetGenericArguments().Single();
			object item = null;
			if (list.Count > header.index)
			{
				item = list[header.index];
			}
			
			SetValue(itemType, ref item, cell);
			if (list.Count <= header.index)
			{
				list.Insert(header.index, item);
			}
		}
		else
		{
			if (null == member)
			{
				member = (MetaData)Activator.CreateInstance(type);
			}
			cell.header = cell.header.child;
			MetaData meta = (MetaData)member;
			FieldInfo childFiledInfo = type.GetField(cell.header.name);
			if (null == childFiledInfo)
			{
				return;
			}

			object childMember = childFiledInfo.GetValue(meta);
			meta.SetValue(childFiledInfo.FieldType, ref childMember, cell);
			childFiledInfo.SetValue(meta, childMember);
		}
	}

	public delegate void ValueTransform(ref object member, string value);
	private Dictionary<string, ValueTransform> bind_functions = new Dictionary<string, ValueTransform>();
	public class Reader<T> : IEnumerable where T : MetaData, new()
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
				meta.OnLoad();
				meta_datas.Add(meta);
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
			Console.WriteLine("column name:" + header.name);
			if (Int32.MaxValue != dotPos)
			{
				header.child = ReadColumnName(cellValue.Substring(dotPos + 1));
			}
			return header;
		}
		public IEnumerator GetEnumerator()
		{
			foreach (T meta in meta_datas)
			{
				yield return meta;
			}
		}
	}
}