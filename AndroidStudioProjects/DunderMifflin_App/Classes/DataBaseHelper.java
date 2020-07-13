package com.example.dm_app;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.widget.Toast;

import androidx.annotation.Nullable;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class DataBaseHelper extends SQLiteOpenHelper {

    public static final String EMPLOYEE_TABLE = "EMPLOYEE_TABLE";
    public static final String COLUMN_EMPLOYEE_FIRST = "EMPLOYEE_FIRST";
    public static final String COLUMN_EMPLOYEE_LAST = "EMPLOYEE_LAST";
    public static final String COLUMN_EMPLOYEE_EMAIL = "EMPLOYEE_EMAIL";
    public static final String COLUMN_EMPLOYEE_PASS = "EMPLOYEE_PASS";
    public static final String COLUMN_IS_MANAGER = "IS_MANAGER";
    public static final String COLUMN_BRANCH = "BRANCH";


    public static final String BRANCH_TABLE = "BRANCH_TABLE";
    public static final String COLUMN_BRANCH_NAME = "BRANCH_NAME";
    public static final String COLUMN_MANAGER_EMAIL = "MANAGER_EMAIL";

    public static final String SALES_TABLE = "SALES_TABLE";
    public static final String COLUMN_ENTRY_ADDED = "ENTRY_ADDED";
    public static final String COLUMN_CLIENT_NAME = "CLIENT_NAME";
    public static final String COLUMN_SALE_AMOUNT = "SALE_AMOUNT";

    public static final String IMAGES_TABLE = "IMAGES_TABLE";
    public static final String COLUMN_IMAGE_BLOB = "IMAGE_BLOB";



    public DataBaseHelper(@Nullable Context context) {
        super(context, "employee.db", null, 1);
    }

    //FIRST TIME DATABASE IS ACCESSED
    @Override
    public void onCreate(SQLiteDatabase db) {
        String createTableStatement = "CREATE TABLE " + EMPLOYEE_TABLE + " (" + COLUMN_EMPLOYEE_FIRST + " TEXT, " + COLUMN_EMPLOYEE_LAST + " TEXT, " + COLUMN_EMPLOYEE_EMAIL + " TEXT PRIMARY KEY, " + COLUMN_EMPLOYEE_PASS + " TEXT, " + COLUMN_IS_MANAGER + " BOOL, " + COLUMN_BRANCH + " INT)";
        db.execSQL(createTableStatement);

        createTableStatement = "CREATE TABLE " + BRANCH_TABLE + " (" + COLUMN_BRANCH_NAME + " TEXT, " + COLUMN_MANAGER_EMAIL + " TEXT PRIMARY KEY)";
        db.execSQL(createTableStatement);

        createTableStatement = "CREATE TABLE " + SALES_TABLE + " (" + COLUMN_ENTRY_ADDED + " INTEGER PRIMARY KEY AUTOINCREMENT, " + COLUMN_EMPLOYEE_EMAIL + " TEXT, "  + COLUMN_CLIENT_NAME + " TEXT, " + COLUMN_SALE_AMOUNT + " FLOAT, " + COLUMN_BRANCH_NAME + " TEXT)";
        db.execSQL(createTableStatement);

        db.execSQL("CREATE TABLE " + IMAGES_TABLE + " (" + COLUMN_EMPLOYEE_EMAIL + " TEXT PRIMARY KEY, " + COLUMN_IMAGE_BLOB + " BLOB NOT NULL)");
    }

    //WHENEVER VERSION IS CHANGED
    @Override
    public void onUpgrade(SQLiteDatabase sqLiteDatabase, int i, int i1) {
        //DROP EXISTING TABLE
        sqLiteDatabase.execSQL("DROP TABLE IF EXISTS " + EMPLOYEE_TABLE);
        sqLiteDatabase.execSQL("DROP TABLE IF EXISTS " + BRANCH_TABLE);
        sqLiteDatabase.execSQL("DROP TABLE IF EXISTS " + SALES_TABLE);
        sqLiteDatabase.execSQL("DROP TABLE IF EXISTS " + IMAGES_TABLE);
    }

    public boolean addOne(EmployeeModel employeeModel){

        SQLiteDatabase db = this.getWritableDatabase();
        ContentValues cv = new ContentValues();

        cv.put(COLUMN_EMPLOYEE_FIRST, employeeModel.getFirstName());
        cv.put(COLUMN_EMPLOYEE_LAST, employeeModel.getLastName());
        cv.put(COLUMN_EMPLOYEE_EMAIL, employeeModel.getEmail());
        cv.put(COLUMN_EMPLOYEE_PASS, employeeModel.getPass());
        cv.put(COLUMN_IS_MANAGER, employeeModel.isManager());
        cv.put(COLUMN_BRANCH, employeeModel.getBranch());
        String branch = convertBranch(employeeModel.getBranch());
        System.out.println("Branch is set to be named: " + branch);
       /*if(checkIfManager(employeeModel.isManager(),employeeModel.getBranch())==false){
            System.out.println("failed here");
            return false;

        }*/
       if(managerForBranch(employeeModel.isManager(),employeeModel.getBranch()) == false){
           System.out.println("Branch = " + employeeModel.getBranch());
           System.out.println("IsManager = " + employeeModel.isManager());
           return false;
       }
        long insert = db.insert(EMPLOYEE_TABLE, null, cv);
        if(insert == -1){
            return false;
        }
        System.out.println("We inserted into first table");
        /*if(checkIfManager(employeeModel.isManager(),employeeModel.getBranch())==true && employeeModel.isManager()){
            ContentValues cv2 = new ContentValues();
            cv2.put(COLUMN_BRANCH_NAME, branch);
            cv2.put(COLUMN_MANAGER_EMAIL, employeeModel.getEmail());

            long insert2 = db.insert(BRANCH_TABLE, null, cv2);
            if(insert2 == -1){
                return false;
            }
        }*/
            if(managerForBranch(employeeModel.isManager(),employeeModel.getBranch())){
                ContentValues cv2 = new ContentValues();
                cv2.put(COLUMN_BRANCH_NAME, branch);
                cv2.put(COLUMN_MANAGER_EMAIL, employeeModel.getEmail());

                long insert2 = db.insert(BRANCH_TABLE, null, cv2);
                if(insert2 == -1){
                    return false;
                }
            }





        return true;
    }
    public boolean addImage(String email, String x){

        SQLiteDatabase db = this.getWritableDatabase();
            try {
                long success;
                FileInputStream fs = new FileInputStream(x);
                byte[] imgByte = new byte[fs.available()];
                fs.read(imgByte);
                ContentValues contentValues = new ContentValues();
                contentValues.put(COLUMN_EMPLOYEE_EMAIL, email);
                contentValues.put(COLUMN_IMAGE_BLOB, imgByte);
                success = db.insert(IMAGES_TABLE, null, contentValues);


                fs.close();
                if (success == -1) {
                    return false;
                }
                return true;
            } catch (IOException e) {
                e.printStackTrace();
            }
            return false;

    }
    public boolean updateImage(String email, String x){
        String queryString = "SELECT * FROM " + IMAGES_TABLE;

        SQLiteDatabase db = this.getWritableDatabase();
        Cursor cursor = db.rawQuery(queryString, null);
        try {
            long success;
            FileInputStream fs = new FileInputStream(x);
            byte[] imgByte = new byte[fs.available()];
            fs.read(imgByte);
            ContentValues contentValues = new ContentValues();
            contentValues.put(COLUMN_EMPLOYEE_EMAIL, email);
            contentValues.put(COLUMN_IMAGE_BLOB, imgByte);
            success = db.update(IMAGES_TABLE, contentValues, "EMPLOYEE_EMAIL = ?", new String[]{email});

            fs.close();
            if (success == -1) {
                return false;
            }
            return true;
        } catch (IOException e) {
            e.printStackTrace();
        }
        return false;


    }

    public boolean imageExists(String email){
        String queryString = "SELECT * FROM " + IMAGES_TABLE;
        boolean exists = false;
        SQLiteDatabase db = this.getReadableDatabase();
        Cursor cursor = db.rawQuery(queryString, null);
        if(cursor.moveToFirst()){
            do{
                String empEmail = cursor.getString(0);
                if(empEmail.equals(email)){
                    exists = true;
                }
            }while(cursor.moveToNext());
        }
        return exists;
    }

    public Bitmap getImage(String email){
        SQLiteDatabase db = this.getWritableDatabase();
        Bitmap bt = null;
        Cursor cursor = db.rawQuery("SELECT * FROM " + IMAGES_TABLE + " WHERE " + COLUMN_EMPLOYEE_EMAIL + "=?", new String[]{email});
        if(cursor.moveToNext()){
            byte[] imag = cursor.getBlob(1);
            bt = BitmapFactory.decodeByteArray(imag, 0, imag.length);
        }
        return bt;
    }

    public boolean isAuth(String email, String password){
        EmployeeModel fromLogin = getFromLogin(email, password);
        if(fromLogin.getFirstName().equals("error") && fromLogin.getLastName().equals("error") && fromLogin.getEmail().equals("error") && fromLogin.getPass().equals("error")){
            return false;
        }
        else{
            return true;
        }

    }
    public EmployeeModel getFromLogin(String email, String password){
        String queryString = "SELECT * FROM " + EMPLOYEE_TABLE;
        String userEmail = email;
        String pass = password;
        SQLiteDatabase db = this.getReadableDatabase();

        Cursor cursor = db.rawQuery(queryString, null);
        if(cursor.moveToFirst()){
            do{
                String employeeFirst = cursor.getString(0);
                String employeeLast = cursor.getString(1);
                String employeeEmail = cursor.getString(2);
                String employeePass = cursor.getString(3);
                boolean employeeManager = cursor.getInt(4) == 1 ? true: false;
                int employeeBranch = cursor.getInt(5);
                if(userEmail.equals(employeeEmail) && employeePass.equals(pass)){
                    EmployeeModel authEmployee = new EmployeeModel(employeeFirst, employeeLast, employeeEmail, employeePass, employeeManager, employeeBranch);
                    return authEmployee;
                }
            }while(cursor.moveToNext());

        }
        EmployeeModel notEmployee = new EmployeeModel("error", "error", "error", "error", false, -1);
        return notEmployee;
    }

    public List<EmployeeModel> getEveryone(){
        List<EmployeeModel> returnList = new ArrayList<>();

        String queryString = "SELECT * FROM " + EMPLOYEE_TABLE;

        SQLiteDatabase db = this.getReadableDatabase();

        Cursor cursor = db.rawQuery(queryString, null);

        if(cursor.moveToFirst()){
            //loop through cursor (results) and create a new customer object then place in return list
            do{
                String employeeFirst = cursor.getString(0);
                String employeeLast = cursor.getString(1);
                String employeeEmail = cursor.getString(2);
                String employeePass = cursor.getString(3);
                boolean employeeManager = cursor.getInt(4) == 1 ? true: false;
                int employeeBranch = cursor.getInt(5);

                EmployeeModel newEmployee = new EmployeeModel(employeeFirst, employeeLast, employeeEmail, employeePass, employeeManager, employeeBranch);
                returnList.add(newEmployee);

            }while(cursor.moveToNext());
        }
        else{
            //failure. do not add anything to the list
        }

        cursor.close();
        db.close();
        return returnList;
    }


    public boolean managerForBranch(boolean checked, int branch){
        String queryString = "SELECT * FROM " + EMPLOYEE_TABLE;
        System.out.println("IsManager = " + checked);
        System.out.println("Branch = " + branch);
        SQLiteDatabase db = this.getReadableDatabase();
        int exists = 0;
        Cursor cursor = db.rawQuery(queryString, null);
        if(checked == false){
            return true;
        }
        if(cursor.moveToFirst()){
            //loop through cursor (results) and create a new customer object then place in return list
            do{
                boolean employeeManager = cursor.getInt(4) == 1 ? true: false;
                int employeeBranch = cursor.getInt(5);
                System.out.println("Found Entry with manager as: " + employeeManager);
                System.out.println("That entry had branch as: " + employeeBranch);
                if(employeeManager == true && employeeBranch == branch){
                    exists = 1;
                }

            }while(cursor.moveToNext());
        }
        else{
            //failure. do not add anything to the list
        }
        System.out.println("Exists = " + exists);

        if(exists == 1){
            return false;
        }
        //cursor.close();
        return true;

    }

    public EmployeeModel getEmployee(String email){
        String queryString = "SELECT * FROM " + EMPLOYEE_TABLE;
        SQLiteDatabase db = this.getReadableDatabase();
        Cursor cursor = db.rawQuery(queryString, null);
        if(cursor.moveToFirst()){
            //loop through cursor (results) and create a new customer object then place in return list
            do{
                String employeeFirst = cursor.getString(0);
                String employeeLast = cursor.getString(1);
                String employeeEmail = cursor.getString(2);
                String employeePass = cursor.getString(3);
                boolean employeeManager = cursor.getInt(4) == 1 ? true: false;
                int employeeBranch = cursor.getInt(5);

                if(employeeEmail.equals(email)){
                    EmployeeModel signedInEmployee = new EmployeeModel(employeeFirst, employeeLast, employeeEmail, employeePass, employeeManager, employeeBranch);
                    return signedInEmployee;
                }
            }while(cursor.moveToNext());
        }
        else{
            //failure. do not add anything to the list
        }
            return new EmployeeModel();
    }

    public List<EmployeeModel> getEveryoneInBranch(int branch){
        List<EmployeeModel> returnList = new ArrayList<>();

        String queryString = "SELECT * FROM " + EMPLOYEE_TABLE;

        SQLiteDatabase db = this.getReadableDatabase();

        Cursor cursor = db.rawQuery(queryString, null);

        if(cursor.moveToFirst()){
            //loop through cursor (results) and create a new customer object then place in return list
            do{
                String employeeFirst = cursor.getString(0);
                String employeeLast = cursor.getString(1);
                String employeeEmail = cursor.getString(2);
                String employeePass = cursor.getString(3);
                boolean employeeManager = cursor.getInt(4) == 1 ? true: false;
                int employeeBranch = cursor.getInt(5);

                EmployeeModel newEmployee = new EmployeeModel(employeeFirst, employeeLast, employeeEmail, employeePass, employeeManager, employeeBranch);

                if(employeeBranch == branch) {
                    returnList.add(newEmployee);
                }

            }while(cursor.moveToNext());
        }
        else{
            //failure. do not add anything to the list
        }

        cursor.close();
        db.close();
        return returnList;
    }
    public List<EmployeeModel> getNone() {
        List<EmployeeModel> returnList = new ArrayList<>();
        return returnList;
    }

    public String convertBranch(int branch){
        if(branch == 1){
            return "Scranton";
        }else if(branch == 2){
            return "Stamford";
        }else if(branch == 3){
            return "Nashua";
        }else{
            return "Unknown";
        }
    }

    public boolean addSale(SaleModel saleModel){
        SQLiteDatabase db = this.getWritableDatabase();
        ContentValues cv = new ContentValues();

        cv.put(COLUMN_EMPLOYEE_EMAIL, saleModel.getEmail());
        cv.put(COLUMN_CLIENT_NAME, saleModel.getClientName());
        cv.put(COLUMN_SALE_AMOUNT, saleModel.getAmount());
        cv.put(COLUMN_BRANCH_NAME, saleModel.getBranchName());

        long insert = db.insert(SALES_TABLE, null, cv);
        if(insert == -1){
            return false;
        }
        //System.out.println("We inserted into Sales table");
        return true;
    }

    public List<SaleModel> getAllSales(EmployeeModel currentEmployee){
        List<SaleModel> returnList = new ArrayList<>();

        String queryString = "SELECT * FROM " + SALES_TABLE;

        SQLiteDatabase db = this.getReadableDatabase();

        Cursor cursor = db.rawQuery(queryString, null);

        if(cursor.moveToFirst()){
            //loop through cursor (results) and create a new customer object then place in return list
            do{
                int saleNumber = cursor.getInt(0);
                String saleEmail = cursor.getString(1);
                String saleClientName = cursor.getString(2);
                float saleAmount = cursor.getFloat(3);
                String saleBranchName = cursor.getString(4);

                SaleModel newSale = new SaleModel(saleNumber, saleEmail, saleClientName, saleAmount, saleBranchName);
                if(currentEmployee.isManager()){
                    if(convertBranch(currentEmployee.getBranch()).equals(saleBranchName)){
                        returnList.add(newSale);
                    }
                }else{
                    if(currentEmployee.getEmail().equals(saleEmail)){
                        returnList.add(newSale);
                    }
                }


            }while(cursor.moveToNext());
        }
        else{
            //failure. do not add anything to the list
        }

        cursor.close();
        db.close();
        return returnList;
    }

    public List<SaleModel> getAllSalesByClient(EmployeeModel currentEmployee){
        List<SaleModel> returnList = new ArrayList<>();
        System.out.println("In getAllSalesByClient");
        String queryString = "SELECT * FROM " + SALES_TABLE + " ORDER BY " + COLUMN_CLIENT_NAME;

        SQLiteDatabase db = this.getReadableDatabase();

        Cursor cursor = db.rawQuery(queryString, null);

        if(cursor.moveToFirst()){
            //loop through cursor (results) and create a new customer object then place in return list
            do{
                int saleNumber = cursor.getInt(0);
                String saleEmail = cursor.getString(1);
                String saleClientName = cursor.getString(2);
                float saleAmount = cursor.getFloat(3);
                String saleBranchName = cursor.getString(4);

                SaleModel newSale = new SaleModel(saleNumber, saleEmail, saleClientName, saleAmount, saleBranchName);
                if(currentEmployee.isManager()){
                    if(convertBranch(currentEmployee.getBranch()).equals(saleBranchName)){
                        returnList.add(newSale);
                    }
                }else{
                    if(currentEmployee.getEmail().equals(saleEmail)){
                        returnList.add(newSale);
                    }
                }


            }while(cursor.moveToNext());
        }
        else{
            //failure. do not add anything to the list
        }


        cursor.close();
        db.close();
        return returnList;
    }

    public List<SaleModel> getAllSalesByAmount_LTH(EmployeeModel currentEmployee){
        List<SaleModel> returnList = new ArrayList<>();
        System.out.println("In getAllSalesByClient");
        String queryString = "SELECT * FROM " + SALES_TABLE + " ORDER BY " + COLUMN_SALE_AMOUNT;

        SQLiteDatabase db = this.getReadableDatabase();

        Cursor cursor = db.rawQuery(queryString, null);

        if(cursor.moveToFirst()){
            //loop through cursor (results) and create a new customer object then place in return list
            do{
                int saleNumber = cursor.getInt(0);
                String saleEmail = cursor.getString(1);
                String saleClientName = cursor.getString(2);
                float saleAmount = cursor.getFloat(3);
                String saleBranchName = cursor.getString(4);

                SaleModel newSale = new SaleModel(saleNumber, saleEmail, saleClientName, saleAmount, saleBranchName);
                if(currentEmployee.isManager()){
                    if(convertBranch(currentEmployee.getBranch()).equals(saleBranchName)){
                        returnList.add(newSale);
                    }
                }else{
                    if(currentEmployee.getEmail().equals(saleEmail)){
                        returnList.add(newSale);
                    }
                }


            }while(cursor.moveToNext());
        }
        else{
            //failure. do not add anything to the list
        }


        cursor.close();
        db.close();
        return returnList;
    }
    public List<SaleModel> getAllSalesByAmount_HTL(EmployeeModel currentEmployee){
        List<SaleModel> returnList = new ArrayList<>();
        System.out.println("In getAllSalesByClient");
        String queryString = "SELECT * FROM " + SALES_TABLE + " ORDER BY " + COLUMN_SALE_AMOUNT + " DESC";

        SQLiteDatabase db = this.getReadableDatabase();

        Cursor cursor = db.rawQuery(queryString, null);

        if(cursor.moveToFirst()){
            //loop through cursor (results) and create a new customer object then place in return list
            do{
                int saleNumber = cursor.getInt(0);
                String saleEmail = cursor.getString(1);
                String saleClientName = cursor.getString(2);
                float saleAmount = cursor.getFloat(3);
                String saleBranchName = cursor.getString(4);

                SaleModel newSale = new SaleModel(saleNumber, saleEmail, saleClientName, saleAmount, saleBranchName);
                if(currentEmployee.isManager()){
                    if(convertBranch(currentEmployee.getBranch()).equals(saleBranchName)){
                        returnList.add(newSale);
                    }
                }else{
                    if(currentEmployee.getEmail().equals(saleEmail)){
                        returnList.add(newSale);
                    }
                }


            }while(cursor.moveToNext());
        }
        else{
            //failure. do not add anything to the list
        }


        cursor.close();
        db.close();
        return returnList;
    }
    public List<SaleModel> getAllSalesByEmployee(EmployeeModel currentEmployee){
        List<SaleModel> returnList = new ArrayList<>();
        System.out.println("In getAllSalesByClient");
        String queryString = "SELECT * FROM " + SALES_TABLE + " ORDER BY " + COLUMN_EMPLOYEE_EMAIL + "," + COLUMN_CLIENT_NAME + "," + COLUMN_SALE_AMOUNT + " ASC";

        SQLiteDatabase db = this.getReadableDatabase();

        Cursor cursor = db.rawQuery(queryString, null);

        if(cursor.moveToFirst()){
            //loop through cursor (results) and create a new customer object then place in return list
            do{
                int saleNumber = cursor.getInt(0);
                String saleEmail = cursor.getString(1);
                String saleClientName = cursor.getString(2);
                float saleAmount = cursor.getFloat(3);
                String saleBranchName = cursor.getString(4);

                SaleModel newSale = new SaleModel(saleNumber, saleEmail, saleClientName, saleAmount, saleBranchName);
                if(currentEmployee.isManager()){
                    if(convertBranch(currentEmployee.getBranch()).equals(saleBranchName)){
                        returnList.add(newSale);
                    }
                }else{
                    if(currentEmployee.getEmail().equals(saleEmail)){
                        returnList.add(newSale);
                    }
                }


            }while(cursor.moveToNext());
        }
        else{
            //failure. do not add anything to the list
        }


        cursor.close();
        db.close();
        return returnList;
    }
    public List<SaleModel> getAllSalesByBranchByID(EmployeeModel currentEmployee){
        List<SaleModel> returnList = new ArrayList<>();
        System.out.println("In getAllSalesByClient");
        String queryString = "SELECT * FROM " + SALES_TABLE + " WHERE " + COLUMN_EMPLOYEE_EMAIL + " = " + currentEmployee.getEmail() + " ORDER BY "  + COLUMN_CLIENT_NAME + "," + COLUMN_SALE_AMOUNT + " ASC";
        queryString += "SELECT * FROM " + SALES_TABLE + " WHERE " + COLUMN_EMPLOYEE_EMAIL + " != " + currentEmployee.getEmail() + " ORDER BY "  + COLUMN_CLIENT_NAME + "," + COLUMN_SALE_AMOUNT + " ASC";

                SQLiteDatabase db = this.getReadableDatabase();

        Cursor cursor = db.rawQuery(queryString, null);

        if(cursor.moveToFirst()){
            //loop through cursor (results) and create a new customer object then place in return list
            do{
                int saleNumber = cursor.getInt(0);
                String saleEmail = cursor.getString(1);
                String saleClientName = cursor.getString(2);
                float saleAmount = cursor.getFloat(3);
                String saleBranchName = cursor.getString(4);

                SaleModel newSale = new SaleModel(saleNumber, saleEmail, saleClientName, saleAmount, saleBranchName);
                if(currentEmployee.isManager()){
                    if(convertBranch(currentEmployee.getBranch()).equals(saleBranchName)){
                        returnList.add(newSale);
                    }
                }else{
                    if(currentEmployee.getEmail().equals(saleEmail)){
                        returnList.add(newSale);
                    }
                }


            }while(cursor.moveToNext());
        }
        else{
            //failure. do not add anything to the list
        }


        cursor.close();
        db.close();
        return returnList;
    }
    public List<SaleModel> getAllBranchSales(int branch) {
        List<SaleModel> returnList = new ArrayList<>();

        String queryString = "SELECT * FROM " + SALES_TABLE;

        SQLiteDatabase db = this.getReadableDatabase();

        Cursor cursor = db.rawQuery(queryString, null);

        if (cursor.moveToFirst()) {
            //loop through cursor (results) and create a new customer object then place in return list
            do {
                int saleNumber = cursor.getInt(0);
                String saleEmail = cursor.getString(1);
                String saleClientName = cursor.getString(2);
                float saleAmount = cursor.getFloat(3);
                String saleBranchName = cursor.getString(4);

                SaleModel newSale = new SaleModel(saleNumber, saleEmail, saleClientName, saleAmount, saleBranchName);
                if(convertBranch(branch).equals(saleBranchName)){
                    returnList.add(newSale);
                }


            } while (cursor.moveToNext());
        } else {
            //failure. do not add anything to the list
        }

        cursor.close();
        db.close();
        return returnList;
    }
}
