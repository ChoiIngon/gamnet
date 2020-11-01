Attribute VB_Name = "Export"
Sub ExportAllSheetsToCSV()
Attribute ExportAllSheetsToCSV.VB_Description = "전체 저장"
Attribute ExportAllSheetsToCSV.VB_ProcData.VB_Invoke_Func = "R\n14"
    Dim sheet As Worksheet
    
        
    For Each sheet In Application.ActiveWorkbook.Worksheets
        Call SaveAsCsv(sheet)
    Next
End Sub

Sub ExportSheetsToCSV()
    Dim sheet As Worksheet
    Set sheet = Application.ActiveSheet
    Call SaveAsCsv(sheet)
End Sub

Sub SaveAsCsv(sheet As Worksheet)
    Dim fileName As String
    Dim i As Integer
    
    For i = 1 To sheet.Columns.Count
        Call CheckType(sheet, i)
    Next
    sheet.Copy
    Application.CutCopyMode = False
    Application.DisplayAlerts = False
    ActiveWorkbook.WebOptions.Encoding = msoEncodingUTF8
    fileName = ThisWorkbook.Path & "\" & Application.ActiveSheet.Name & ".csv"
    ActiveWorkbook.SaveAs fileName:=fileName, FileFormat:=xlCSVUTF8
    Application.DisplayAlerts = True
    ActiveWindow.Close
End Sub

Sub CheckType(sheet As Worksheet, column As Integer)
    columnType = LCase(sheet.Cells(2, column).value)
    Dim i As Integer
    Dim rowCount As Integer
    Dim value As String
    rowCount = sheet.Cells(sheet.Rows.Count, column).End(xlUp).Row
    Select Case columnType
    Case "datetime"
        For i = 3 To rowCount
           value = sheet.Cells(i, column).value
           If "" <> value And False = IsDate(value) Then
                MsgBox "worng date format row_num:" & i & ", column:" & column
                Exit Sub
            End If
        Next i
    Case "int"
        For i = 3 To rowCount
            value = sheet.Cells(i, column).value
            If "" <> value And False = IsNumeric(value) Then
                MsgBox "worng Int format row_num:" & i & ", column:" & column
                Exit Sub
            End If
        Next
    End Select
End Sub

