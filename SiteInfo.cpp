#include "SiteInfo.h"


bool SiteInfo::open()
{
    pages.clear();

    if(!std::ifstream(".siteinfo/pages.list"))
    {
        //this should never happen!
        std::cout << "ERROR: SiteInfo.h: could not open site information as .siteinfo/pages.list does not exist" << std::endl;
        return 1;
    }

    std::ifstream ifs(".siteinfo/pages.list");
    PageInfo inPage;
    while(ifs >> inPage)
    {
        //checks that content and template files aren't the same
        if(inPage.contentPath == inPage.templatePath)
        {
            std::cout << "error: failed to open .siteinfo/pages.list" << std::endl;
            std::cout << "reason: page " << inPage.pagePath << " has same content and template path" << inPage.templatePath << std::endl;

            return 1;
        }

        //makes sure there's no duplicate entries in pages.list
        if(pages.count(inPage) > 0)
        {
            PageInfo cInfo = *(pages.find(inPage));

            std::cout << "error: failed to open .siteinfo/pages.list" << std::endl;
            std::cout << "reason: duplicate entry for " << inPage.pagePath << std::endl;
            std::cout << std::endl;
            std::cout << "----- first entry -----" << std::endl;
            std::cout << "   page title: " << cInfo.pageTitle << std::endl;
            std::cout << "    page path: " << cInfo.pagePath << std::endl;
            std::cout << " content path: " << cInfo.contentPath << std::endl;
            std::cout << "template path: " << cInfo.templatePath << std::endl;
            std::cout << "--------------------------------" << std::endl;
            std::cout << std::endl;
            std::cout << "----- second entry -----" << std::endl;
            std::cout << "   page title: " << inPage.pageTitle << std::endl;
            std::cout << "    page path: " << inPage.pagePath << std::endl;
            std::cout << " content path: " << inPage.contentPath << std::endl;
            std::cout << "template path: " << inPage.templatePath << std::endl;
            std::cout << "--------------------------------" << std::endl;

            return 1;
        }

        pages.insert(inPage);
    }

    return 0;
};

bool SiteInfo::save()
{
    std::ofstream ofs(".siteinfo/pages.list");

    for(auto page=pages.begin(); page != pages.end(); page++)
    {
        ofs << page->pageTitle << std::endl;
        ofs << page->pagePath << std::endl;
        ofs << page->contentPath << std::endl;
        ofs << page->templatePath << std::endl;
        ofs << std::endl;
    }

    return 0;
};

bool SiteInfo::tracking(const PageInfo &page)
{
    return pages.count(page);
};

bool SiteInfo::tracking(const Path &pagePath)
{
    PageInfo page;
    page.pagePath = pagePath;
    return pages.count(page);
};

PageInfo SiteInfo::get_tracked(const Path &pagePath)
{
    PageInfo page;
    page.pagePath = pagePath;
    return *pages.find(page);
};

bool SiteInfo::tracked()
{
    std::cout << std::endl;
    std::cout << "------ tracked pages ------" << std::endl;
    for(auto page=pages.begin(); page!=pages.end(); page++)
    {
        if(page != pages.begin())
            std::cout << std::endl;
        std::cout << "   page title: " << page->pageTitle << std::endl;
        std::cout << "    page path: " << page->pagePath << std::endl;
        std::cout << " content path: " << page->contentPath << std::endl;
        std::cout << "template path: " << page->templatePath << std::endl;
    }
    std::cout << "-----------------------------" << std::endl;

    return 0;
};

bool SiteInfo::tracked_paths()
{
    std::cout << std::endl;
    std::cout << "--------- tracked page paths ---------" << std::endl;
    for(auto page=pages.begin(); page!=pages.end(); page++)
        std::cout << page->pagePath << std::endl;
    std::cout << "--------------------------------------" << std::endl;

    return 0;
};

bool SiteInfo::track(const PageInfo &newPage)
{
    if(newPage.contentPath == newPage.templatePath)
    {
        std::cout << std::endl;
        std::cout << "error: content and template paths cannot be the same, page not tracked" << std::endl;
        return 1;
    }

    if(pages.count(newPage) > 0)
    {
        PageInfo cInfo = *(pages.find(newPage));

        std::cout << std::endl;
        std::cout << "error: nsm is already tracking " << newPage.pagePath << std::endl;
        std::cout << "----- current page details -----" << std::endl;
        std::cout << "   page title: " << cInfo.pageTitle << std::endl;
        std::cout << "    page path: " << cInfo.pagePath << std::endl;
        std::cout << " content path: " << cInfo.contentPath << std::endl;
        std::cout << "template path: " << cInfo.templatePath << std::endl;
        std::cout << "--------------------------------" << std::endl;

        return 1;
    }

    //warns user if content and/or template paths don't exist
    if(!std::ifstream(newPage.contentPath.str))
    {
        std::cout << std::endl;
        std::cout << "warning: content path " << newPage.contentPath << " does not exist" << std::endl;
    }
    if(!std::ifstream(newPage.templatePath.str))
    {
        std::cout << std::endl;
        std::cout << "warning: template path " << newPage.templatePath << " does not exist" << std::endl;
    }

    //inserts new page into set of pages
    pages.insert(newPage);

    //saves new set of pages to pages.list
    std::ofstream ofs(".siteinfo/pages.list");
    for(auto page=pages.begin(); page!=pages.end(); page++)
        ofs << *page << std::endl << std::endl;
    ofs.close();

    //informs user that page addition was successful
    std::cout << std::endl;
    std::cout << "successfully tracking " << newPage.pagePath << std::endl;

    return 0;
};

bool SiteInfo::untrack(const Path &pagePathToUntrack)
{
    //checks that page is being tracked
    if(!tracking(pagePathToUntrack))
    {
        std::cout << std::endl;
        std::cout << "error: nsm is not tracking " << pagePathToUntrack << std::endl;
        return 1;
    }
    else
    {
        PageInfo pageToErase;
        pageToErase.pagePath = pagePathToUntrack;

        //removes page info file
        std::string systemCall = "rm -f " + pagePathToUntrack.getInfoPath().str;
        system(systemCall.c_str());

        //removes page from pages set
        pages.erase(pageToErase);

        //saves new set of pages to pages.list
        std::ofstream ofs(".siteinfo/pages.list");
        for(auto page=pages.begin(); page!=pages.end(); page++)
            ofs << *page << std::endl << std::endl;
        ofs.close();

        //informs user that page was successfully untracked
        std::cout << std::endl;
        std::cout << "successfully untracked " << pagePathToUntrack << std::endl;
    }
};

int SiteInfo::build(std::vector<Path> pagePathsToBuild)
{
    PageBuilder pageBuilder;
    std::set<Path> untrackedPages, failedPages;

    for(auto pagePath=pagePathsToBuild.begin(); pagePath != pagePathsToBuild.end(); pagePath++)
    {
        if(tracking(*pagePath))
        {
            if(pageBuilder.build(get_tracked(*pagePath)) > 0)
                failedPages.insert(*pagePath);
        }
        else
            untrackedPages.insert(*pagePath);
    }

    if(failedPages.size() > 0)
    {
        std::cout << std::endl;
        std::cout << "---- following pages failed to build ----" << std::endl;
        for(auto fPath=failedPages.begin(); fPath != failedPages.end(); fPath++)
            std::cout << " " << *fPath << std::endl;
        std::cout << "-----------------------------------------" << std::endl;
    }
    if(untrackedPages.size() > 0)
    {
        std::cout << std::endl;
        std::cout << "---- nsm not tracking following pages ----" << std::endl;
        for(auto uPath=untrackedPages.begin(); uPath != untrackedPages.end(); uPath++)
            std::cout << *uPath << std::endl;
        std::cout << "------------------------------------------" << std::endl;
    }
    if(failedPages.size() == 0 && untrackedPages.size() == 0)
    {
        std::cout << std::endl;
        std::cout << "all pages built successfully" << std::endl;
    }
};

int SiteInfo::build_all()
{
    PageBuilder pageBuilder;

    if(pages.size() == 0)
    {
        std::cout << std::endl;
        std::cout << "nsm is not tracking any pages, nothing to build" << std::endl;
        return 0;
    }

    std::set<Path> failedPages;

    for(auto page=pages.begin(); page != pages.end(); page++)
        if(pageBuilder.build(*page) > 0)
            failedPages.insert(page->pagePath);

    if(failedPages.size() > 0)
    {
        std::cout << std::endl;
        std::cout << "---- following pages failed to build ----" << std::endl;
        for(auto fPath=failedPages.begin(); fPath != failedPages.end(); fPath++)
            std::cout << " " << *fPath;
        std::cout << std::endl;
        std::cout << "-----------------------------------------" << std::endl;
    }
    else
    {
        std::cout << std::endl;
        std::cout << "all pages built successfully" << std::endl;
    }

    return 0;
};

bool SiteInfo::build_updated()
{
    PageBuilder pageBuilder;
    std::set<PageInfo> updatedPages;
    std::set<Path> modifiedFiles,
        removedFiles,
        builtPages,
        problemPages;

    std::cout << std::endl;
    for(auto page=pages.begin(); page != pages.end(); page++)
    {
        //checks whether content and template files exist
        if(!std::ifstream(page->contentPath.str))
        {
            std::cout << page->pagePath << ": content file " << page->contentPath << " does not exist" << std::endl;
            problemPages.insert(page->pagePath);
            continue;
        }
        if(!std::ifstream(page->templatePath.str))
        {
            std::cout << page->pagePath << ": template file " << page->templatePath << " does not exist" << std::endl;
            problemPages.insert(page->pagePath);
            continue;
        }

        //gets path of pages information from last time page was built
        Path pageInfoPath = page->pagePath.getInfoPath();

        //checks whether info path exists
        if(!std::ifstream(pageInfoPath.str))
        {
            std::cout << page->pagePath << ": yet to be built" << std::endl;
            updatedPages.insert(*page);
            continue;
        }
        else
        {
            std::ifstream infoStream(pageInfoPath.str);
            PageInfo prevPageInfo;
            infoStream >> prevPageInfo;

            if(page->pageTitle != prevPageInfo.pageTitle)
            {
                std::cout << page->pagePath << ": title changed to " << page->pageTitle << " from " << prevPageInfo.pageTitle << std::endl;
                updatedPages.insert(*page);
                continue;
            }

            if(page->contentPath != prevPageInfo.contentPath)
            {
                std::cout << page->pagePath << ": content path changed to " << page->contentPath << " from " << prevPageInfo.contentPath << std::endl;
                updatedPages.insert(*page);
                continue;
            }

            if(page->templatePath != prevPageInfo.templatePath)
            {
                std::cout << page->pagePath << ": template path changed to " << page->templatePath << " from " << prevPageInfo.templatePath << std::endl;
                updatedPages.insert(*page);
                continue;
            }

            Path dep;
            while(infoStream >> dep)
            {
                if(!std::ifstream(dep.str))
                {
                    std::cout << page->pagePath << ": dep path " << dep << " removed since last build" << std::endl;
                    removedFiles.insert(dep);
                    updatedPages.insert(*page);
                    break;
                }
                else if(dep.modified_after(pageInfoPath))
                {
                    std::cout << page->pagePath << ": dep path " << dep << " modified since last build" << std::endl;
                    modifiedFiles.insert(dep);
                    updatedPages.insert(*page);
                    break;
                }
            }
        }
    }

    if(removedFiles.size() > 0)
    {
        std::cout << std::endl;
        std::cout << "---- removed content files ----" << std::endl;
        for(auto rFile=removedFiles.begin(); rFile != removedFiles.end(); rFile++)
            std::cout << *rFile << std::endl;
        std::cout << "-------------------------------" << std::endl;
    }

    if(modifiedFiles.size() > 0)
    {
        std::cout << std::endl;
        std::cout << "------- updated content files ------" << std::endl;
        for(auto uFile=modifiedFiles.begin(); uFile != modifiedFiles.end(); uFile++)
            std::cout << *uFile << std::endl;
        std::cout << "------------------------------------" << std::endl;
    }

    if(updatedPages.size() > 0)
    {
        std::cout << std::endl;
        std::cout << "----- pages that need building -----" << std::endl;
        for(auto uPage=updatedPages.begin(); uPage != updatedPages.end(); uPage++)
            std::cout << uPage->pagePath << std::endl;
        std::cout << "------------------------------------" << std::endl;
    }

    if(problemPages.size() > 0)
    {
        std::cout << std::endl;
        std::cout << "----- pages with missing content or template file -----" << std::endl;
        for(auto pPage=problemPages.begin(); pPage != problemPages.end(); pPage++)
            std::cout << *pPage << std::endl;
        std::cout << "-------------------------------------------------------" << std::endl;
    }
    problemPages.clear();

    for(auto uPage=updatedPages.begin(); uPage != updatedPages.end(); uPage++)
    {
        if(pageBuilder.build(*uPage) > 0)
            problemPages.insert(uPage->pagePath);
        else
            builtPages.insert(uPage->pagePath);
    }

    if(builtPages.size() > 0)
    {
        std::cout << std::endl;
        std::cout << "----- pages successfully built -----" << std::endl;
        for(auto bPage=builtPages.begin(); bPage != builtPages.end(); bPage++)
            std::cout << *bPage << std::endl;
        std::cout << "------------------------------------" << std::endl;
    }

    if(problemPages.size() > 0)
    {
        std::cout << std::endl;
        std::cout << "----- pages that failed to build -----" << std::endl;
        for(auto pPage=problemPages.begin(); pPage != problemPages.end(); pPage++)
            std::cout << *pPage << std::endl;
        std::cout << "--------------------------------------" << std::endl;
    }

    if(updatedPages.size() == 0)
    {
        //std::cout << std::endl;
        std::cout << "all pages are already up to date" << std::endl;
    }
};

bool SiteInfo::status()
{
    std::set<Path> updatedFiles, removedFiles;
    std::set<PageInfo> updatedPages, problemPages;

    for(auto page=pages.begin(); page != pages.end(); page++)
    {
        bool needsUpdating = 0;

        //checks whether content and template files exist
        if(!std::ifstream(page->contentPath.str))
        {
            needsUpdating = 1; //shouldn't need this but doesn't cost much
            problemPages.insert(*page);
            //note user will be informed about this as a dep not existing
        }
        if(!std::ifstream(page->templatePath.str))
        {
            needsUpdating = 1; //shouldn't need this but doesn't cost much
            problemPages.insert(*page);
            //note user will be informed about this as a dep not existing
        }

        //gets path of pages information from last time page was built
        Path pageInfoPath = page->pagePath.getInfoPath();

        //checks whether info path exists
        if(!std::ifstream(pageInfoPath.str))
        {
            std::cout << page->pagePath << ": yet to be built" << std::endl;
            needsUpdating = 1;
        }
        else
        {
            std::ifstream infoStream(pageInfoPath.str);
            PageInfo prevPageInfo;
            infoStream >> prevPageInfo;

            if(page->pageTitle != prevPageInfo.pageTitle)
            {
                std::cout << page->pagePath << ": title changed to " << page->pageTitle << " from " << prevPageInfo.pageTitle << std::endl;
                needsUpdating = 1;
            }

            if(page->contentPath != prevPageInfo.contentPath)
            {
                std::cout << page->pagePath << ": content path changed to " << page->contentPath << " from " << prevPageInfo.contentPath << std::endl;
                needsUpdating = 1;
            }

            if(page->templatePath != prevPageInfo.templatePath)
            {
                std::cout << page->pagePath << ": template path changed to " << page->templatePath << " from " << prevPageInfo.templatePath << std::endl;
                needsUpdating = 1;
            }

            Path dep;
            while(infoStream >> dep)
            {
                if(!std::ifstream(dep.str))
                {
                    removedFiles.insert(dep);
                    needsUpdating = 1;
                }
                else if(dep.modified_after(pageInfoPath))
                {
                    updatedFiles.insert(dep);
                    needsUpdating = 1;
                }
            }
        }

        if(needsUpdating && problemPages.count(*page) == 0)
            updatedPages.insert(*page);
    }

    if(removedFiles.size() > 0)
    {
        std::cout << std::endl;
        std::cout << "---- removed content files ----" << std::endl;
        for(auto rFile=removedFiles.begin(); rFile != removedFiles.end(); rFile++)
            std::cout << *rFile << std::endl;
        std::cout << "-------------------------------" << std::endl;
    }

    if(updatedFiles.size() > 0)
    {
        std::cout << std::endl;
        std::cout << "------- updated content files ------" << std::endl;
        for(auto uFile=updatedFiles.begin(); uFile != updatedFiles.end(); uFile++)
            std::cout << *uFile << std::endl;
        std::cout << "------------------------------------" << std::endl;
    }

    if(updatedPages.size() > 0)
    {
        std::cout << std::endl;
        std::cout << "----- pages that need building -----" << std::endl;
        for(auto uPage=updatedPages.begin(); uPage != updatedPages.end(); uPage++)
            std::cout << uPage->pagePath << std::endl;
        std::cout << "------------------------------------" << std::endl;
    }

    if(problemPages.size() > 0)
    {
        std::cout << std::endl;
        std::cout << "----- pages with missing content or template file -----" << std::endl;
        for(auto uPage=problemPages.begin(); uPage != problemPages.end(); uPage++)
            std::cout << uPage->pagePath << std::endl;
        std::cout << "-------------------------------------------------------" << std::endl;
    }

    if(updatedFiles.size() == 0 && updatedPages.size() == 0 && problemPages.size() == 0)
    {
        std::cout << std::endl;
        std::cout << "all pages are already up to date" << std::endl;
    }
};